#pragma once

#include "BuildingDef.h"

/*
*   
*/

class BuildingHarvesterDef : public BuildingDef
  {
public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual GameObjectType getType() const override { return GameObjectType::harvester; }

  };
