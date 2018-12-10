//
// Created by matt on 1/12/18.
//

#include <Unit.h>
#include <GameObjectDefFileHelper.h>
#include <Behaviours/SettlerBehaviour.h>
#include "UnitDef.h"

bool UnitDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!DynamicObjectDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlUnit = xmlGameObjectDef->FirstChildElement(OD_UNIT);
  if (xmlUnit)
    {
    xmlUnit->QueryAttribute(OD_SPEED, &speed);
    }

  return true;
  }

SMGameActorPtr UnitDef::createGameActor(GameContext* gameContext) const
  {
  Unit* actor = new Unit(gameContext->getNextActorID(), this);
  actor->setSpeed(speed);
  actor->setMaxCapacity(1, false);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }

void UnitDef::createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const
  {
  IGameObjectDef::createActorBehaviours(behaviourList);
  behaviourList->push_back(IGameObjectBehaviourPtr(new SettlerBehaviour()));
  }

