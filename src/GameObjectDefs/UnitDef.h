#pragma once

#include "DynamicObjectDef.h"

/*
*   
*/

class UnitDef : public DynamicObjectDef
  {
public:
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;
  virtual GameObjectType getType() const override { return GameObjectType::unit; }
  };
