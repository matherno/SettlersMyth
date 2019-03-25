//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include <Resources.h>
#include "ComponentStorage.h"


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
      resCollectCycle.push_back(blueprint->id);
    });

  std::shuffle(resCollectCycle.begin(), resCollectCycle.end(), std::default_random_engine());
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