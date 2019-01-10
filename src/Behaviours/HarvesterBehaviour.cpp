//
// Created by matt on 4/12/18.
//

#include <Building.h>
#include <GameObjectDefs/BuildingHarvesterDef.h>
#include <GameObjectDefs/StaticObjectDef.h>
#include <SMGameContext.h>
#include "HarvesterBehaviour.h"

void HarvesterBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {
  sendUnitTimer.setTimeOut(2000);
  sendUnitTimer.reset();

  resourceID = BuildingHarvesterDef::getHarvesterDepositResourceID(gameActor, gameContext);
  }

void HarvesterBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (sendUnitTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    Building* building = Building::cast(gameActor);
    if (!building)
      {
      ASSERT(false, "");
      return;
      }

    if (gameActor->canStoreResource(resourceID, 1))
      {
      UnitPtr unit = building->getIdleUnit();
      if (unit)
        unit->processCommand(SMActorCommand(CMD_HARVEST), gameContext);
      }

    building->returnIdleUnits(gameActor, gameContext);
    sendUnitTimer.reset();
    }
  }

void HarvesterBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

