#pragma once

#include "GameObjectDefBase.h"

/*
*   
*/

class DynamicObjectDef : public GameObjectDefBase
  {
public:
  string renderableFilePath;
  bool isMeshRenderable = true;   // otherwise voxels

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual RenderablePtr constructRenderable(RenderContext* renderContext, const Vector3D& translation, uint meshIdx = 0) const override;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;
  virtual uint getMeshCount() const override { return 1; }

  static const DynamicObjectDef* cast(const IGameObjectDef* def) { return dynamic_cast<const DynamicObjectDef*>(def); }
  };
