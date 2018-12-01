//
// Created by matt on 1/12/18.
//

#include <Unit.h>
#include <GameObjectDefFileHelper.h>
#include "UnitDef.h"

SMGameActorPtr UnitDef::createGameActor(GameContext* gameContext) const
  {
  Unit* actor = new Unit(gameContext->getNextActorID(), this);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }
