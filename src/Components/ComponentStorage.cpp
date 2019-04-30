//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include <Resources.h>
#include "ComponentStorage.h"
#include "UIResourceAmount.h"


/*
*   ComponentStorageBlueprint
*/

bool ComponentStorageBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  return true;
  }

bool ComponentStorageBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  return true;
  }

SMComponentPtr ComponentStorageBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentStorage(actor, type, this));
  }



/*
*   ComponentStorage
*/

ComponentStorage::ComponentStorage(const SMGameActorPtr& actor, SMComponentType type, const ComponentStorageBlueprint* blueprint) : BuildingComponent(actor, type), blueprint(blueprint)
  {}

void ComponentStorage::initialise(GameContext* gameContext)
  {
  collectResTimer.setTimeOut(SEND_SETTLER_COOLDOWN);
  collectResTimer.reset();
  
  resCollectIdx = 0;
  resCollectCycle.clear();
  SMGameContext::cast(gameContext)->getGameObjectFactory()->forEachGameActorBlueprint([&](const SMGameActorBlueprint* blueprint)
    {
    if (blueprint->type == SMGameActorType::resource)
      resourcesCanCollect[blueprint->id] = true;
    });
  resetResCollectCycleArray();
  }

void ComponentStorage::initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent)
  {
  initialise(gameContext);
  
  if (xmlComponent)
    {
    XMLElement* xmlResource = xmlComponent->FirstChildElement(SL_RESOURCE);
    while (xmlResource)
      {
      string resName = xmlGetStringAttribute(xmlResource, SL_NAME);
      const bool collectRes = xmlResource->BoolAttribute(SL_COLLECT, true);
      const SMGameActorBlueprint* resBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(resName);
      if (resBlueprint && resourcesCanCollect.count(resBlueprint->id) > 0)
        resourcesCanCollect[resBlueprint->id] = collectRes;
      xmlResource = xmlResource->NextSiblingElement(SL_RESOURCE);
      }
    resetResCollectCycleArray();
    }
  }

void ComponentStorage::save(GameContext* gameContext, XMLElement* xmlComponent)
  {
  for (auto& pair : resourcesCanCollect)
    {
    const SMGameActorBlueprint* resBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->getGameActorBlueprint(pair.first);
    if (!resBlueprint)
      continue;
    XMLElement* xmlResource = xmlCreateElement(xmlComponent, SL_RESOURCE);
    xmlResource->SetAttribute(SL_NAME, resBlueprint->name.c_str());
    xmlResource->SetAttribute(SL_COLLECT, pair.second);
    }
  }

void ComponentStorage::update(GameContext* gameContext)
  {
  //  collect resources if got space
  if (collectResTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    sendUnitToCollect(gameContext);
    collectResTimer.reset();
    }
  }

void ComponentStorage::cleanUp(GameContext* gameContext)
  {
  
  }

void ComponentStorage::sendUnitToCollect(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  //  get an idle attached unit
  GameActorUnit* unit = building->getIdleUnit().get();
  if (!unit)
    return;

  //  find which resource is available somewhere to collect, and we can also store it
  SMGameActorPtr actorToCollectFrom;
  uint resToCollect = 0;
  int numResChecked = 0;
  while (!actorToCollectFrom && numResChecked < resCollectCycle.size())
    {
    resToCollect = resCollectCycle[resCollectIdx];
    resCollectIdx++;
    if (resCollectIdx >= resCollectCycle.size())
      resCollectIdx = 0;
    numResChecked++;
    if (building->canStoreResource(resToCollect, 1))
      {
      actorToCollectFrom = smGameContext->getGridMapHandler()->findClosestGriddedActor(gameContext, building->getEntryPosition(), 
        [&](SMGameActorPtr actor)
        {
        if (actor->getID() == building->getID() || actor->gotComponentType(SMComponentType::buildingStorage))
          return false;
        const SMGameActorBlueprint* blueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(actor->getBlueprintTypeID());
        if (!blueprint || blueprint->type != SMGameActorType::building)
          return false;
        //  todo: allow resource pickup from floor
        return actor->resourceCount(resToCollect) >= 1;
        });
      }
    }

  // then tell the idle unit to get this resource
  if (actorToCollectFrom)
    {
    UnitCommandPtr command(new CollectResourceUnitCommand(actorToCollectFrom->getID(), resToCollect));
    unit->performCommand(command);
    }
  }

int ComponentStorage::onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset)
  {
  uiResourceList.reset(new UIResourceAmountList(gameContext->getUIManager()->getNextComponentID(), getActor().get(), 3));
  for (auto& pair : resourcesCanCollect)
    uiResourceList->addResource(pair.first, gameContext, pair.second);
  uiResourceList->setOffset(Vector2D(0, yOffset));
  uiResourceList->setHorizontalAlignment(Alignment::alignmentCentre);
  uiResourceList->setOnResourceToggledFunc([this](bool toggledOn, uint resID)
    {
    resourcesCanCollect[resID] = toggledOn;
    resetResCollectCycleArray();
    });
  parentPanel->addChild(uiResourceList);
  return uiResourceList->getPixelHeightUsed();
  }

void ComponentStorage::onUpdateSelectionHUD(GameContext* gameContext) 
  {
  uiResourceList->updateResources();
  }

void ComponentStorage::resetResCollectCycleArray()
  {
  resCollectCycle.clear();
  for (auto& pair : resourcesCanCollect)
    {
    if (pair.second)  
      resCollectCycle.push_back(pair.first);
    }
  std::shuffle(resCollectCycle.begin(), resCollectCycle.end(), std::default_random_engine());
  }