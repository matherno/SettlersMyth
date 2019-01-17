//
// Created by matt on 16/12/18.
//

#include <SMGameContext.h>
#include "SettlerCollectResourceBehaviour.h"


void SettlerCollectResourceBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (!isBehaviourActive())
    return;

  auto target = getTargetBuilding();
  auto attachedBuilding = getAttachedBuilding(gameActor, gameContext);

  if (!target)
    {
    if (attachedBuilding)
      attachedBuilding->freeResourceSpace(collectResSpaceReserve);
    endBehaviour(gameActor, gameContext, true);
    return;
    }

  if (!attachedBuilding)
    {
    if (target)
      target->unlockResource(collectResLock);
    endBehaviour(gameActor, gameContext, true);
    return;
    }

  Unit* unit = Unit::cast(gameActor);
  if (unit->hasReachedTarget())
    {
    attachedBuilding->freeResourceSpace(collectResSpaceReserve);
    if(target->takeLockedResource(collectResLock))
      unit->storeResource(collectResLock.resID, collectResLock.resAmount);
    targetBuilding.reset();
    endBehaviour(gameActor, gameContext, true);
    }
  else if (unit->canNotReachTarget())
    {
    attachedBuilding->freeResourceSpace(collectResSpaceReserve);
    target->unlockResource(collectResLock);
    targetBuilding.reset();
    endBehaviour(gameActor, gameContext, true);
    }
  }

bool SettlerCollectResourceBehaviour::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  if (SettlerBehaviourBase::processCommand(gameActor, gameContext, command))
    return true;

  if (command.id == CMD_COLLECT || command.id == CMD_COLLECT_NOT_STORAGE)
    {
    const GridMapHandler* gridMapHandler = SMGameContext::cast(gameContext)->getGridMapHandler();
    const GameObjectFactory* gameObjectFactory = SMGameContext::cast(gameContext)->getGameObjectFactory();
    const uint resToCollect = (uint) command.extra;

    auto attachedBuilding = getAttachedBuilding(gameActor, gameContext);
    if (!attachedBuilding)
      return false;
    collectResSpaceReserve = attachedBuilding->reserveResourceSpace(resToCollect, 1);
    if (!collectResSpaceReserve.isValid())
      return false;

    targetBuilding = gridMapHandler->findClosestStaticActor(gameContext, Unit::cast(gameActor)->getPosition(),
      [&](SMStaticActorPtr actor)
      {
      if (actor->getID() == attachedBuilding->getID())
        return false;
      if (gameObjectFactory->isTypeOrSubType(GameObjectType::deposit, actor->getDef()->getType()))
        return false;
      if (command.id == CMD_COLLECT_NOT_STORAGE && gameObjectFactory->isTypeOrSubType(GameObjectType::storage, actor->getDef()->getType()))
        return false;

      return actor->resourceCount(resToCollect) > 0;
      });

    if (getTargetBuilding())
      {
      Unit::cast(gameActor)->setTarget(getTargetBuilding()->getEntryPosition().centre());
      collectResLock = getTargetBuilding()->lockResource(resToCollect, 1);
      ASSERT(collectResLock.isValid(), "");
      startBehaviour(gameActor, gameContext, command.id);
      }
    return true;
    }
  return false;
  }


Building* SettlerCollectResourceBehaviour::getTargetBuilding()
  {
  return Building::cast(targetBuilding.lock().get());
  }

void SettlerCollectResourceBehaviour::onCancelBehaviour(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (getTargetBuilding())
    getTargetBuilding()->unlockResource(collectResLock);
  if (getAttachedBuilding(gameActor, gameContext))
    getAttachedBuilding(gameActor, gameContext)->freeResourceSpace(collectResSpaceReserve);
  SettlerBehaviourBase::onCancelBehaviour(gameActor, gameContext);
  }
