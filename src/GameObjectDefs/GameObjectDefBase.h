#pragma once

#include "SMGameObjectFactory.h"

/*
*   
*/

class GameObjectDefBase : public IGameObjectDef
  {
private:
  uint id;
  string name;
  string iconFilepath;

public:
  virtual string getName() const override { return name; }
  virtual uint getID() const override { return id; }
  virtual string getIconFilePath() const override { return iconFilepath; }
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  };
