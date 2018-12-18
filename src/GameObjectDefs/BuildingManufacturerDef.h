#pragma once


#include "BuildingDef.h"

/*
*   
*/

class BuildingManufacturerDef : public BuildingDef
  {
public:
  std::map<string, uint> inputs;
  string outputResName;
  uint outputResAmount = 0;
  uint manufactureTime = 0;
  GridXY manufactureSpot;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual GameObjectType getType() const override { return GameObjectType::manufacturer; }
  static const BuildingManufacturerDef* cast(const IGameObjectDef* def) { return dynamic_cast<const BuildingManufacturerDef*>(def); }

protected:
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const override;
  };
