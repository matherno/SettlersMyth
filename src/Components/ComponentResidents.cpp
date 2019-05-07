//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentResidents.h"
#include "UIResourceAmount.h"


/*
*   ComponentResidentsBlueprint
*/

bool ComponentResidentsBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  maxUnits = xmlComponent->IntAttribute(OD_MAXUNITS, 10);
  unitBlueprintName = xmlGetStringAttribute(xmlComponent, OD_UNIT);
  return true;
  }

bool ComponentResidentsBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  const SMGameActorBlueprint* unitBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(unitBlueprintName);
  if (!unitBlueprint)
    {
    *errorMsg = "Unit name '" + unitBlueprintName + "' unknown.";
    return false;
    }
  unitBlueprintID = unitBlueprint->id;
  return true;
  }

SMComponentPtr ComponentResidentsBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentResidents(actor, type, this));
  }




/*
*   ComponentResidents
*/

ComponentResidents::ComponentResidents(const SMGameActorPtr& actor, SMComponentType type, const ComponentResidentsBlueprint* blueprint) : BuildingComponent(actor, type), blueprint(blueprint)
  {}

void ComponentResidents::initialise(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  if (!building->getIsUnderConstruction())
    populateResidents(gameContext);
  }

void ComponentResidents::initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent)
  {
  }

void ComponentResidents::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  BuildingComponent::onMessage(gameContext, message, extra);

  switch (message)
    {
    case SMMessage::constructionFinished:
      populateResidents(gameContext);
      break;
    }
  }

void ComponentResidents::update(GameContext* gameContext)
  {
  }

void ComponentResidents::cleanUp(GameContext* gameContext)
  {
  
  }

void ComponentResidents::populateResidents(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  for (int i = 0; i < blueprint->maxUnits; ++i)
    {
    SMGameActorPtr actor = smGameContext->createSMGameActor(blueprint->unitBlueprintID, building->getEntryPosition());
    GameActorUnitPtr unit = std::dynamic_pointer_cast<GameActorUnit>(actor);
    building->addResidentUnit(unit);
    }
  }

int ComponentResidents::onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset)
  {
  GameActorBuilding* building = getBuildingActor();
  const SMGameActorBlueprint* unitBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->getGameActorBlueprint(blueprint->unitBlueprintID);
  if (!unitBlueprint)
    return 0;

  uiOccupancyCount.reset(new UIResourceAmount(gameContext->getUIManager()->getNextComponentID(), unitBlueprint->iconPath, building->getResidentUnitCount()));
  uiOccupancyCount->setOffset(Vector2D(0, yOffset));
  uiOccupancyCount->setHorizontalAlignment(alignmentCentre);
  parentPanel->addChild(uiOccupancyCount);
  return uiOccupancyCount->getSize().y;
  }

void ComponentResidents::onUpdateSelectionHUD(GameContext* gameContext) 
  {
  GameActorBuilding* building = getBuildingActor();
  if (uiOccupancyCount)
    uiOccupancyCount->updateAmount(building->getResidentUnitCount());
  }
