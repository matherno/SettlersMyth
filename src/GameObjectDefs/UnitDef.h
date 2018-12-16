#pragma once

#include "DynamicObjectDef.h"

/*
*   
*/

class UnitDef : public DynamicObjectDef
  {
public:
  double speed = 1;
  Vector3D pickupPos;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;
  virtual GameObjectType getType() const override { return GameObjectType::unit; }

  static const UnitDef* cast(const IGameObjectDef* def) { return dynamic_cast<const UnitDef*>(def); }

protected:
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const override;

  };
