#pragma once

#include "StaticObjectDef.h"
#include "Grid.h"

/*
*   
*/



class BuildingDef : public StaticObjectDef
  {
public:
  GridXY buildingEntry;
  std::vector<std::pair<string, int>> constructionReq;
  int constructionTime = 0;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;

  static const BuildingDef* cast(const IGameObjectDef* def) { return dynamic_cast<const BuildingDef*>(def); }
  };
