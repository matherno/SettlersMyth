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

    Unit* unit = building->getIdleUnit();
    if (unit)
      unit->processCommand(SMActorCommand(CMD_HARVEST), gameContext);
    returnIdlesToBase(gameActor, gameContext);
    sendUnitTimer.reset();
    }
  }

void HarvesterBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

void HarvesterBehaviour::returnIdlesToBase(SMGameActor* gameActor, GameContext* gameContext)
  {
  Building* building = Building::cast(gameActor);
  for (auto unitPtr : *building->getAttachedUnits()->getList())
    {
    if(Unit* unit = unitPtr.lock().get())
      {
      if(unit->isIdling() && building->getGridPosition() != GridXY(unit->getPosition()))
        unit->processCommand(CMD_RETURN_TO_BASE, gameContext);
      }
    }
  }

