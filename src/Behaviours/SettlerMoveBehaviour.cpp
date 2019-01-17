//
// Created by matt on 16/12/18.
//

#include "SettlerMoveBehaviour.h"
#include <Unit.h>
#include <SMGameContext.h>
#include <GameObjectDefs/BuildingManufacturerDef.h>


void SettlerMoveBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (!isBehaviourActive())
    return;

  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    {
    endBehaviour(gameActor, gameContext);
    return;
    }

  Unit* unit = Unit::cast(gameActor);
  if (unit->hasReachedTarget())
    {
    if (transferResourcesToBase)
      {
      freeReservedResSpace(gameActor, gameContext);
      unit->transferAllResourcesTo(buildingActor);
      }
    const double angleRotation = -mathernogl::ccwAngleBetween(Vector2D(0, 1), targetFaceDirection);
    unit->setRotation(angleRotation);
    endBehaviour(gameActor, gameContext);
    }
  else if (unit->canNotReachTarget())
    {
    freeReservedResSpace(gameActor, gameContext);
    endBehaviour(gameActor, gameContext);
    }
  }

bool SettlerMoveBehaviour::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  if (SettlerBehaviourBase::processCommand(gameActor, gameContext, command))
    return true;

  if (command.id == CMD_RETURN_TO_BASE)
    {
    auto attachedBuilding = getAttachedBuilding(gameActor, gameContext);
    if (!attachedBuilding)
      return false;

    gameActor->forEachResource([&](uint id, uint amount)
        {
        ResourceReserve reserve = attachedBuilding->reserveResourceSpace(id, amount);
        if (reserve.isValid())
          baseResSpaceReserves.push_back(reserve);
        });

    transferResourcesToBase = true;
    targetFaceDirection.x = 0;
    targetFaceDirection.y = 1;
    Unit::cast(gameActor)->setTarget(attachedBuilding->getEntryPosition().centre());
    startBehaviour(gameActor, gameContext, command.id);
    return true;
    }

  if (command.id == CMD_MOVE_TO_MANUF_SPOT)
    {
    auto buildingActor = getAttachedBuilding(gameActor, gameContext);
    if (!buildingActor)
      return false;
    auto manufDef = BuildingManufacturerDef::cast(buildingActor->getDef());
    if (!manufDef)
      return false;
    Unit::cast(gameActor)->setTarget(buildingActor->getGridPosition() + manufDef->manufactureSpot.centre());
    targetFaceDirection = manufDef->manufactureDir;
    transferResourcesToBase = false;
    startBehaviour(gameActor, gameContext, command.id);
    return true;
    }
  return false;
  }

void SettlerMoveBehaviour::freeReservedResSpace(SMGameActor* gameActor, GameContext* gameContext)
  {
  SMGameActor* attachedBuilding = getAttachedBuilding(gameActor, gameContext);
  if (!attachedBuilding)
    return;
  for (ResourceReserve reserve : baseResSpaceReserves)
    attachedBuilding->freeResourceSpace(reserve);
  baseResSpaceReserves.clear();
  }

void SettlerMoveBehaviour::onCancelBehaviour(SMGameActor* gameActor, GameContext* gameContext)
  {
  freeReservedResSpace(gameActor, gameContext);
  SettlerBehaviourBase::onCancelBehaviour(gameActor, gameContext);
  }

