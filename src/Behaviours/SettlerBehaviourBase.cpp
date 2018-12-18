//
// Created by matt on 16/12/18.
//

#include <SMGameContext.h>
#include "SettlerBehaviourBase.h"



Building* SettlerBehaviourBase::getAttachedBuilding(SMGameActor* gameActor, GameContext* gameContext)
  {
  Unit* unit = Unit::cast(gameActor);
  const uint attachedBuildingLinkID = unit->getAttachedBuilding();
  return Building::cast(SMGameContext::cast(gameContext)->getStaticActor(attachedBuildingLinkID).get());
  }

bool SettlerBehaviourBase::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  if (command.id == CMD_IDLE)
    {
    if (performingCommand)
      endBehaviour(gameActor, gameContext, false);
    return true;
    }
  return false;
  }

void SettlerBehaviourBase::endBehaviour(SMGameActor* gameActor, GameContext* gameContext, bool returnToBase)
  {
  performingCommand = false;
  Unit::cast(gameActor)->clearTarget();
  Unit::cast(gameActor)->setIdle(true);
  const uint finishCmd = activeCommand;
  if (returnToBase)
    gameActor->processCommand(CMD_RETURN_TO_BASE, gameContext);
  gameActor->notifyObserversCommandFinish(gameContext, finishCmd);
  }

void SettlerBehaviourBase::startBehaviour(SMGameActor* gameActor, GameContext* gameContext, uint commandID)
  {
  performingCommand = true;
  Unit::cast(gameActor)->setIdle(false);
  gameActor->notifyObserversCommandStart(gameContext, commandID);
  activeCommand = commandID;
  }