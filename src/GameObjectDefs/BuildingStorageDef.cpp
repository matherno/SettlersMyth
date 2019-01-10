//
// Created by matt on 9/01/19.
//

#include "BuildingStorageDef.h"
#include <Behaviours/ResourceStackBehaviour.h>
#include <Behaviours/StorageBehaviour.h>
#include "GameObjectDefFileHelper.h"


bool BuildingStorageDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!BuildingDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;



  return true;
  }

void BuildingStorageDef::createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const
  {
  IGameObjectDef::createActorBehaviours(behaviourList);
  behaviourList->push_back(IGameObjectBehaviourPtr(new StorageBehaviour()));
  behaviourList->push_back(IGameObjectBehaviourPtr(new ResourceStackBehaviour()));
  }

