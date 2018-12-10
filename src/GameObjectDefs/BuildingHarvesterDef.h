#pragma once

#include "BuildingDef.h"

/*
*   
*/

class BuildingHarvesterDef : public BuildingDef
  {
public:
  string depositName;
  int harvestTime = 1000;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual GameObjectType getType() const override { return GameObjectType::harvester; }
  static const BuildingHarvesterDef* cast(const IGameObjectDef* def) { return dynamic_cast<const BuildingHarvesterDef*>(def); }

protected:
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const override;
  };
