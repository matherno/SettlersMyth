//
// Created by matt on 9/12/18.
//

#include <Unit.h>
#include <SMGameContext.h>
#include <GameObjectDefs/BuildingHarvesterDef.h>
#include "SettlerHarvestBehaviour.h"


void SettlerHarvestBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (!isBehaviourActive())
    return;

  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return;

  if (!getTargetActor() || getTargetActor()->totalResourceCount() == 0)
    {
    endBehaviour(gameActor, gameContext, true);
    return;
    }

  Unit* unit = Unit::cast(gameActor);

  if (unit->hasReachedTarget())
    {
    if (timer.incrementTimer(gameContext->getDeltaTime()))
      {
      const uint resourceID = BuildingHarvesterDef::getHarvesterDepositResourceID(buildingActor, gameContext);
      if(getTargetActor()->takeResource(resourceID, 1))
        unit->storeResource(resourceID, 1);
      timer.reset();
      endBehaviour(gameActor, gameContext, true);
      }
    }
  else if (unit->canNotReachTarget())
    {
    timer.reset();
    endBehaviour(gameActor, gameContext, true);
    }
  }


bool SettlerHarvestBehaviour::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  if (SettlerBehaviourBase::processCommand(gameActor, gameContext, command))
    return true;

  if (command.id != CMD_HARVEST)
    return false;

  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return false;

  auto harvesterDef = BuildingHarvesterDef::cast(buildingActor->getDef());
  if (!harvesterDef)
    return false;

  Unit* unit = Unit::cast(gameActor);
  unit->dropAllResources(gameContext, unit->getPosition());

  const string& depositName = harvesterDef->depositName;
  const GridXY& buildingPosition = buildingActor->getGridPosition();
  targetActor = smGameContext->getGridMapHandler()->findClosestStaticActor(gameContext, unit->getPosition(), depositName);

  if (getTargetActor())
    {
    timer.setTimeOut(harvesterDef->harvestTime);
    timer.reset();
    unit->setTarget(getTargetActor()->getPosition());
    startBehaviour(gameActor, gameContext, command.id);
    return true;
    }
  else
    return false;
  }


SMStaticActor* SettlerHarvestBehaviour::getTargetActor()
  {
  return targetActor.lock().get();
  }
