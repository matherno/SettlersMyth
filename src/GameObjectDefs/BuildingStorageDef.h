#pragma once

#include "BuildingDef.h"

/*
*   
*/

class BuildingStorageDef : public BuildingDef
  {
public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual GameObjectType getType() const override { return GameObjectType::storage; }
  static const BuildingStorageDef* cast(const IGameObjectDef* def) { return dynamic_cast<const BuildingStorageDef*>(def); }

protected:
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const override;
  };
