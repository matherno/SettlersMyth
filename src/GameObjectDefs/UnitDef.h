#pragma once

#include "DynamicObjectDef.h"

/*
*   
*/

class UnitDef : public DynamicObjectDef
  {
public:
  double speed = 1;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;
  virtual GameObjectType getType() const override { return GameObjectType::unit; }

protected:
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const override;

  };
