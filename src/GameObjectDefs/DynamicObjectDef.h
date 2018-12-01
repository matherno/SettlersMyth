#pragma once

#include "GameObjectDefBase.h"

/*
*   
*/

class DynamicObjectDef : public GameObjectDefBase
  {
private:
  double speed = 1;
  string meshFilePath;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual RenderablePtr constructRenderable(RenderContext* renderContext) const override;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;

  double getSpeed() const { return speed; }

  static const DynamicObjectDef* cast(const IGameObjectDef* def) { return dynamic_cast<const DynamicObjectDef*>(def); }
  };
