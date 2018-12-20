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
  behaviourList->push_back(IGameObjectBehaviourPtr(new ResourceDepositeBehaviour()));
  }

SMGameActorPtr ResourceDepositDef::createGameActor(GameContext* gameContext) const
  {
  auto actor = StaticObjectDef::createGameActor(gameContext);
  if (actor)
    {
    auto resourceGameObjDef = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDef(resourceName);
    if (resourceGameObjDef)
      actor->storeResource(resourceGameObjDef->getID(), resourceAmount);
    else
      ASSERT(false, "Not a resource? '" + resourceName + "'");
    }
  return actor;
  }





void ResourceDepositeBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {
  float xPos = mathernogl::RandomGenerator::randomFloat(0.15, 0.85);
  float yPos = mathernogl::RandomGenerator::randomFloat(0.15, 0.85);
  SMStaticActor* staticActor = SMStaticActor::cast(gameActor);
  if (staticActor)
    staticActor->setCellPos(Vector2D(xPos, yPos));
  }

void ResourceDepositeBehaviour::initialiseFromSaved(SMGameActor* gameActor, GameContext* gameContext, XMLElement* xmlElement)
  {
  }

void ResourceDepositeBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (gameActor->totalResourceCount() == 0)
    SMGameContext::cast(gameContext)->destroySMActor(gameActor->getID());
  }

void ResourceDepositeBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

string ResourceDepositeBehaviour::getBehaviourName()
  {
  return "ResourceDeposit";
  }
