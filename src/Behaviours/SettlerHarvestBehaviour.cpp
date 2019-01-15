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
    unit->lookAt(getTargetActor()->getPosition());

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

static const float harvestOffsetAmt = 0.475;
static const std::vector<Vector2D> depositHarvestOffsets =
  {
  Vector2D(harvestOffsetAmt,  0),
  Vector2D(harvestOffsetAmt,  harvestOffsetAmt),
  Vector2D(harvestOffsetAmt,  -harvestOffsetAmt),
  Vector2D(-harvestOffsetAmt, 0),
  Vector2D(-harvestOffsetAmt, harvestOffsetAmt),
  Vector2D(-harvestOffsetAmt, -harvestOffsetAmt),
  Vector2D(0,                 harvestOffsetAmt),
  Vector2D(0,                 -harvestOffsetAmt),
  };

bool SettlerHarvestBehaviour::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  if (SettlerBehaviourBase::processCommand(gameActor, gameContext, command))
    return true;

  if (command.id != CMD_HARVEST)
    return false;

  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  SMGameActor* buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return false;

  const BuildingHarvesterDef* harvesterDef = BuildingHarvesterDef::cast(buildingActor->getDef());
  if (!harvesterDef)
    return false;

  Unit* unit = Unit::cast(gameActor);
  unit->dropAllResources(gameContext, unit->getPosition());

  const string& depositName = harvesterDef->depositName;
  const GridXY& buildingPosition = buildingActor->getGridPosition();
  const Vector2D harvestOffset = depositHarvestOffsets[mathernogl::RandomGenerator::randomInt(0, (int)depositHarvestOffsets.size() - 1)];
  targetActor = smGameContext->getGridMapHandler()->findClosestStaticActor(gameContext, unit->getPosition(), depositName);

  if (getTargetActor())
    {
    timer.setTimeOut(harvesterDef->harvestTime);
    timer.reset();
    unit->setTarget(getTargetActor()->getPosition() + harvestOffset);
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
