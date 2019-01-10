//
// Created by matt on 24/11/18.
//

#include <GameObjectDefFileHelper.h>
#include <SMGameContext.h>
#include "ResourceDepositDef.h"

bool ResourceDepositDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!StaticObjectDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlDeposit = xmlGameObjectDef->FirstChildElement(OD_DEPOSIT);
  if (xmlDeposit)
    {
    resourceName = xmlGetStringAttribute(xmlDeposit, OD_RESOURCENAME);
    xmlDeposit->QueryAttribute(OD_RESOURCEAMOUNT, &resourceAmount);
    }
  else
    {
    *errorMsg = "No deposit definition found";
    return false;
    }

  return true;
  }

void ResourceDepositDef::createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const
  {
  IGameObjectDef::createActorBehaviours(behaviourList);
  behaviourList->push_back(IGameObjectBehaviourPtr(new ResourceDepositBehaviour()));
  }

SMGameActorPtr ResourceDepositDef::createGameActor(GameContext* gameContext) const
  {
  SMGameActorPtr actor = StaticObjectDef::createGameActor(gameContext);
  actor->setupStackCount(1, resourceAmount);
  return actor;
  }





void ResourceDepositBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {
  const ResourceDepositDef* resDef = ResourceDepositDef::cast(gameActor->getDef());
  if (!resDef)
    return;

  IGameObjectDefPtr resourceGameObjDef = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDef(resDef->resourceName);
  if (resourceGameObjDef)
    gameActor->storeResource(resourceGameObjDef->getID(), resDef->resourceAmount);
  else
    ASSERT(false, "Not a resource? '" + resDef->resourceName + "'");
  }

void ResourceDepositBehaviour::initialiseFromSaved(SMGameActor* gameActor, GameContext* gameContext, XMLElement* xmlElement)
  {
  }

void ResourceDepositBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (gameActor->totalResourceCount() == 0)
    SMGameContext::cast(gameContext)->destroySMActor(gameActor->getID());
  }

void ResourceDepositBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

string ResourceDepositBehaviour::getBehaviourName()
  {
  return "ResourceDepositBehaviour";
  }

