//
// Created by matt on 10/11/18.
//

#include <Behaviours/HarvesterBehaviour.h>
#include <Behaviours/ResourceStackBehaviour.h>
#include <SMGameContext.h>
#include "BuildingHarvesterDef.h"
#include "GameObjectDefFileHelper.h"
#include "ResourceDepositDef.h"

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


/*static*/ uint BuildingHarvesterDef::getHarvesterDepositResourceID(SMGameActor* gameActor, GameContext* gameContext)
  {
  const BuildingHarvesterDef* harvesterDef = BuildingHarvesterDef::cast(gameActor->getDef());
  if (harvesterDef)
    {
    const ResourceDepositDef* depositDef = ResourceDepositDef::cast(SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDef(harvesterDef->depositName).get());
    if (depositDef)
      return SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDefID(depositDef->resourceName);
    }
  }

