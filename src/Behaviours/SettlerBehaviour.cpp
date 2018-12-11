//
// Created by matt on 9/12/18.
//

#include <Unit.h>
#include <SMGameContext.h>
#include <GameObjectDefs/BuildingHarvesterDef.h>
#include "SettlerBehaviour.h"

void SettlerBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

void SettlerBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  bool unitBusy = false;
  if (activeCommand == CMD_HARVEST)
    unitBusy = updateHarvesting(gameActor, gameContext);
  else if (activeCommand == CMD_RETURN_TO_BASE)
    unitBusy = updateReturnToBase(gameActor, gameContext);


  Unit::cast(gameActor)->setIdle(!unitBusy);
  if (!unitBusy)
    {
    activeCommand = CMD_IDLE;
    Unit::cast(gameActor)->clearTarget();
    }
  }

void SettlerBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

bool SettlerBehaviour::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  activeCommand = command.id;
  if (command.id == CMD_HARVEST)
    {
    if(startHarvesting(gameActor, gameContext))
      return true;
    }
  else if (command.id == CMD_RETURN_TO_BASE)
    {
    if(startReturnToBase(gameActor, gameContext))
      return true;
    }
  else if (command.id == CMD_IDLE)
    {
    Unit::cast(gameActor)->setIdle(true);
    return true;
    }

  activeCommand = CMD_IDLE;

  return false;
  }

Building* SettlerBehaviour::getAttachedBuilding(SMGameActor* gameActor, GameContext* gameContext)
  {
  Unit* unit = Unit::cast(gameActor);
  const uint attachedBuildingLinkID = unit->getAttachedBuilding();
  return Building::cast(SMGameContext::cast(gameContext)->getStaticActor(attachedBuildingLinkID).get());
  }

SMStaticActor* SettlerBehaviour::getTargetActor()
  {
  return targetActor.lock().get();
  }



/*
 *  Harvesting
 */

bool SettlerBehaviour::startHarvesting(SMGameActor* gameActor, GameContext* gameContext)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return false;

  auto harvesterDef = BuildingHarvesterDef::cast(buildingActor->getDef());
  if (!harvesterDef)
    return false;

  Unit* unit = Unit::cast(gameActor);
  unit->dropAllResources(gameContext, unit->getPosition());

  const GridXY& buildingPosition = buildingActor->getGridPosition();
  const string& depositName = harvesterDef->depositName;
  targetActor = smGameContext->getGridMapHandler()->findClosestStaticActor(gameContext, buildingPosition, depositName);

  if (getTargetActor())
    {
    timer.setTimeOut(harvesterDef->harvestTime);
    timer.reset();
    unit->setTarget(getTargetActor()->getPosition());
    return true;
    }
  else
    {
    processCommand(gameActor, gameContext, CMD_RETURN_TO_BASE);
    return true;
    }
  }

bool SettlerBehaviour::updateHarvesting(SMGameActor* gameActor, GameContext* gameContext)
  {
  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return false;

  auto harvesterDef = BuildingHarvesterDef::cast(buildingActor->getDef());
  if (!harvesterDef)
    return false;

  if (!getTargetActor() || getTargetActor()->getStoredResources()->empty())
    return false;

  Unit* unit = Unit::cast(gameActor);

  if (unit->hasReachedTarget())
    {
    if (timer.incrementTimer(gameContext->getDeltaTime()))
      {
      const uint resourceID = getTargetActor()->getStoredResources()->begin()->first;
      if(getTargetActor()->takeResource(resourceID, 1))
        unit->storeResource(resourceID, 1);
      timer.reset();
      processCommand(gameActor, gameContext, CMD_RETURN_TO_BASE);
      }
    }
  return true;
  }



/*
 *  Returning to base
 */

bool SettlerBehaviour::startReturnToBase(SMGameActor* gameActor, GameContext* gameContext)
  {
  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return false;
  Unit::cast(gameActor)->setTarget(buildingActor->getGridPosition());
  return true;
  }

bool SettlerBehaviour::updateReturnToBase(SMGameActor* gameActor, GameContext* gameContext)
  {
  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return false;
  Unit* unit = Unit::cast(gameActor);
  if (unit->hasReachedTarget())
    {
    unit->transferAllResourcesTo(buildingActor);
    return false;
    }
  return true;
  }
