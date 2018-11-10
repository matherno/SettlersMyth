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
  };
