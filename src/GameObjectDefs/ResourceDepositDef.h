#pragma once

#include "StaticObjectDef.h"

/*
*   
*/

class ResourceDepositDef : public StaticObjectDef
  {
public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual GameObjectType getType() const override { return GameObjectType::deposit; }

protected:
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const override;
  };
