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
  if (!target)
    {
    endBehaviour(gameActor, gameContext, true);
    return;
    }

  Unit* unit = Unit::cast(gameActor);
  if (unit->hasReachedTarget())
    {
    if(target->takeResource(resToCollect, 1))
      unit->storeResource(resToCollect, 1);
    endBehaviour(gameActor, gameContext, true);
    }
  else if (unit->canNotReachTarget())
    {
    endBehaviour(gameActor, gameContext, true);
    }
  }

bool SettlerCollectResourceBehaviour::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  if (SettlerBehaviourBase::processCommand(gameActor, gameContext, command))
    return true;

  if (command.id == CMD_COLLECT)
    {
    const GridMapHandlerBase* gridMapHandler = SMGameContext::cast(gameContext)->getGridMapHandler();
    const GameObjectFactory* gameObjectFactory = SMGameContext::cast(gameContext)->getGameObjectFactory();
    resToCollect = (uint) command.extra;
    auto attachedBuilding = getAttachedBuilding(gameActor, gameContext);
    if (!attachedBuilding)
      return false;

    targetBuilding = gridMapHandler->findClosestStaticActor(gameContext, Unit::cast(gameActor)->getPosition(), [&](SMStaticActorPtr actor)
      {
      if (actor->getID() == attachedBuilding->getID())
        return false;
      if (gameObjectFactory->isTypeOrSubType(GameObjectType::deposit, actor->getDef()->getType()))
        return false;
      return actor->resourceCount(resToCollect) > 0;
      });

    if (getTargetBuilding())
      {
      //todo reserve the resource to take from target, and the space in the attached building
      Unit::cast(gameActor)->setTarget(getTargetBuilding()->getEntryPosition().centre());
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