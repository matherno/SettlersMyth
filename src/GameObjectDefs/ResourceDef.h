#pragma once

#include "DynamicObjectDef.h"

/*
*   
*/

class ResourceDef : public DynamicObjectDef
  {
public:
  std::vector<std::pair<Vector3D, double>> stackPosAndRots;

  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual GameObjectType getType() const override { return GameObjectType::resource; }

  static const ResourceDef* cast(const IGameObjectDef* def){ return dynamic_cast<const ResourceDef*>(def); }
  };
