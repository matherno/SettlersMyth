//
// Created by matt on 9/01/19.
//

#include "StorageBehaviour.h"
#include <Building.h>
#include <GameObjectDefs/BuildingStorageDef.h>
#include <GameObjectDefs/StaticObjectDef.h>
#include <SMGameContext.h>


void StorageBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {
  sendUnitTimer.setTimeOut(2000);
  sendUnitTimer.reset();

  SMGameContext::cast(gameContext)->getGameObjectFactory()->forEachGameObjectDef(GameObjectType::resource,
    [this](IGameObjectDefPtr defPtr)
    {
    resourcesToStore.push_back(defPtr->getID());
    });
  }

void StorageBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (resourcesToStore.size() == 0)
    return;

  if (sendUnitTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    Building* building = Building::cast(gameActor);
    if (!building)
      {
      ASSERT(false, "");
      return;
      }

    uint resToCollect = 0;
    int numResChecked = 0;
    while (numResChecked < resourcesToStore.size())
      {
      resToCollect = resourcesToStore[nextResToCollect];
      nextResToCollect++;
      if (nextResToCollect >= resourcesToStore.size())
        nextResToCollect = 0;
      numResChecked++;
      if (gameActor->canStoreResource(resToCollect, 1))
        break;
      else
        resToCollect = 0;
      }

    if (resToCollect > 0)
      {
      UnitPtr unit = building->getIdleUnit();
      if (unit)
        unit->processCommand(SMActorCommand(CMD_COLLECT_NOT_STORAGE, resToCollect), gameContext);
      }

    building->returnIdleUnits(gameActor, gameContext);
    sendUnitTimer.reset();
    }
  }

void StorageBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

