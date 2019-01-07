//
// Created by matt on 10/11/18.
//

#include <Behaviours/HarvesterBehaviour.h>
#include <Behaviours/ResourceStackBehaviour.h>
#include "BuildingHarvesterDef.h"
#include "GameObjectDefFileHelper.h"

bool BuildingHarvesterDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!BuildingDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  if (auto xmlHarvester = xmlGameObjectDef->FirstChildElement(OD_HARVESTER))
    {
    if (auto xmlDeposit = xmlHarvester->FirstChildElement(OD_DEPOSIT))
      {
      depositName = xmlGetStringAttribute(xmlDeposit, OD_NAME);
      xmlDeposit->QueryAttribute(OD_HARVESTTIME, &harvestTime);
      }
    }
  else
    {
    *errorMsg = "No harvester definition found";
    return false;
    }

  return true;
  }

void BuildingHarvesterDef::createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const
  {
  IGameObjectDef::createActorBehaviours(behaviourList);
  behaviourList->push_back(IGameObjectBehaviourPtr(new HarvesterBehaviour()));
  behaviourList->push_back(IGameObjectBehaviourPtr(new ResourceStackBehaviour()));
  }

