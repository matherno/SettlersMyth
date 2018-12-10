#pragma once

#include "DynamicObjectDef.h"

/*
*   
*/

class ResourceDef : public DynamicObjectDef
  {
public:
  virtual GameObjectType getType() const override { return GameObjectType::resource; }
  };
