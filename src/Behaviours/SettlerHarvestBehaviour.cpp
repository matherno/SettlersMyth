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

  auto attachedBuilding = getAttachedBuilding(gameActor, gameContext);

  if (!attachedBuilding || !getTargetActor())
    {
    if (getTargetActor())
      getTargetActor()->unlockResource(depositResLock);
    if (attachedBuilding)
      attachedBuilding->freeResourceSpace(baseResReserve);
    targetActor.reset();
    endBehaviour(gameActor, gameContext, true);
    return;
    }

  Unit* unit = Unit::cast(gameActor);

  if (unit->hasReachedTarget())
    {
    unit->lookAt(getTargetActor()->getPosition());

    if (timer.incrementTimer(gameContext->getDeltaTime()))
      {
      attachedBuilding->freeResourceSpace(baseResReserve);
      if(getTargetActor()->takeLockedResource(depositResLock))
        unit->storeResource(depositResLock.resID, depositResLock.resAmount);
      timer.reset();
      targetActor.reset();
      endBehaviour(gameActor, gameContext, true);
      }
    }
  else if (unit->canNotReachTarget())
    {
    attachedBuilding->freeResourceSpace(baseResReserve);
    getTargetActor()->unlockResource(depositResLock);
    timer.reset();
    targetActor.reset();
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
  SMGameActor* attachedBuilding = getAttachedBuilding(gameActor, gameContext);
  if (!attachedBuilding)
    return false;

  const BuildingHarvesterDef* harvesterDef = BuildingHarvesterDef::cast(attachedBuilding->getDef());
  if (!harvesterDef)
    return false;

  const uint resourceID = BuildingHarvesterDef::getHarvesterDepositResourceID(attachedBuilding, gameContext);
  const string& depositName = harvesterDef->depositName;
  const GridXY& buildingPosition = attachedBuilding->getGridPosition();
  const Vector2D harvestOffset = depositHarvestOffsets[mathernogl::RandomGenerator::randomInt(0, (int)depositHarvestOffsets.size() - 1)];

  baseResReserve = attachedBuilding->reserveResourceSpace(resourceID, 1);
  if (!baseResReserve.isValid())
    return false;

  Unit* unit = Unit::cast(gameActor);
  unit->dropAllResources(gameContext, unit->getPosition());

  targetActor = smGameContext->getGridMapHandler()->findClosestStaticActor(gameContext, unit->getPosition(),
                  [&](SMStaticActorPtr actor)
                    {
                    if (actor->getDef()->getUniqueName() != depositName)
                      return false;
                    return actor->resourceCount(resourceID) > 0;
                    });

  if (getTargetActor())
    {
    depositResLock = getTargetActor()->lockResource(resourceID, 1);
    ASSERT(depositResLock.isValid(), "");

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

void SettlerHarvestBehaviour::onCancelBehaviour(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (getTargetActor())
    getTargetActor()->unlockResource(depositResLock);
  auto attachedBuilding = getAttachedBuilding(gameActor, gameContext);
  if (attachedBuilding)
    attachedBuilding->freeResourceSpace(baseResReserve);
  SettlerBehaviourBase::onCancelBehaviour(gameActor, gameContext);
  }
