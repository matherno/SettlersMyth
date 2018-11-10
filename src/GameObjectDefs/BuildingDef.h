#pragma once

#include "StaticObjectDef.h"
#include "Grid.h"

/*
*   
*/



class BuildingDef : public StaticObjectDef
  {
private:
  std::vector<GridXY> inputSpots;
  std::vector<GridXY> outputSpots;
  GridXY lotEntry;
  GridXY lotEntryDir;
  GridXY buildingEntry;
  GridXY buildingEntryDir;
  std::vector<std::pair<string, int>> constructionReq;
  int constructionTime = 0;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;

  const std::vector<GridXY>* getInputSpots() const { return &inputSpots; }
  const std::vector<GridXY>* getOutputSpots() const { return &outputSpots; }
  GridXY getLotEntry() const { return lotEntry; }
  GridXY getLotEntryDir() const { return lotEntryDir; }
  GridXY getBuildingEntry() const { return buildingEntry; }
  GridXY getBuildingEntryDir() const { return buildingEntryDir; }
  };
