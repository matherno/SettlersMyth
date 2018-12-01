#pragma once

#include <Grid.h>
#include "GameObjectDefBase.h"

/*
*   
*/

class StaticObjectDef : public GameObjectDefBase
  {
private:
  GridXY size;
  string meshFilePath;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual RenderablePtr constructRenderable(RenderContext* renderContext) const override;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;
  GridXY getSize() const { return size; }
  int getSizeX() const { return size.x; }
  int getSizeY() const { return size.y; }

  static const StaticObjectDef* cast(const IGameObjectDef* def) { return dynamic_cast<const StaticObjectDef*>(def); }
  };

class ObstacleDef : public StaticObjectDef
  {
public:
  virtual GameObjectType getType() const override { return GameObjectType::obstacle; }
  static const ObstacleDef* cast(const IGameObjectDef* def) { return dynamic_cast<const ObstacleDef*>(def); }
  };
