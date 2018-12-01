//
// Created by matt on 24/11/18.
//

#include "ResourceDepositDef.h"

bool ResourceDepositDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!StaticObjectDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;



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

  IGameObjectBehaviourPtr randomCellPos(new BehaviourHelper(onInitFunc, nullptr));
  behaviourList->push_back(randomCellPos);
  }

