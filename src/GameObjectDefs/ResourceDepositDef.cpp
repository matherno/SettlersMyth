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

  auto onInitFunc = [](SMGameActor* gameActor, GameContext* gameContext)
    {
    float xPos = mathernogl::RandomGenerator::randomFloat(0, 1);
    float yPos = mathernogl::RandomGenerator::randomFloat(0, 1);
    SMStaticActor* staticActor = SMStaticActor::cast(gameActor);
    if (staticActor)
      staticActor->setCellPos(Vector2D(xPos, yPos));
    };

  auto onUpdateFunc = [](SMGameActor* gameActor, GameContext* gameContext)
    {
    if (gameActor->totalResourceCount() == 0)
      SMGameContext::cast(gameContext)->destroySMActor(gameActor->getID());
    };

  IGameObjectBehaviourPtr randomCellPos(new BehaviourHelper(onInitFunc, onUpdateFunc));
  behaviourList->push_back(randomCellPos);
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

