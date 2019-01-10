//
// Created by matt on 1/12/18.
//

#include <Unit.h>
#include <GameObjectDefFileHelper.h>
#include <Behaviours/SettlerHarvestBehaviour.h>
#include <Behaviours/CarryResourceBehaviour.h>
#include <Behaviours/SettlerMoveBehaviour.h>
#include <Behaviours/SettlerCollectResourceBehaviour.h>
#include <Behaviours/SettlerManufactureBehaviour.h>
#include "UnitDef.h"

bool UnitDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!DynamicObjectDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlUnit = xmlGameObjectDef->FirstChildElement(OD_UNIT);
  if (xmlUnit)
    {
    xmlUnit->QueryAttribute(OD_SPEED, &speed);
    pickupPos = xmlGetVec3Value(xmlUnit, OD_PICKUP);
    }

  return true;
  }

SMGameActorPtr UnitDef::createGameActor(GameContext* gameContext) const
  {
  Unit* actor = new Unit(gameContext->getNextActorID(), this);
  actor->setSpeed(speed);
  actor->setupStackCount(1, 1);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }

void UnitDef::createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const
  {
  IGameObjectDef::createActorBehaviours(behaviourList);
  behaviourList->push_back(IGameObjectBehaviourPtr(new SettlerMoveBehaviour()));
  behaviourList->push_back(IGameObjectBehaviourPtr(new SettlerCollectResourceBehaviour()));
  behaviourList->push_back(IGameObjectBehaviourPtr(new SettlerHarvestBehaviour()));
  behaviourList->push_back(IGameObjectBehaviourPtr(new SettlerManufactureBehaviour()));
  behaviourList->push_back(IGameObjectBehaviourPtr(new CarryResourceBehaviour()));
  }

