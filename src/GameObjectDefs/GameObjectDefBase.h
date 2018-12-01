#pragma once

#include "SMGameObjectFactory.h"

/*
*   
*/

class GameObjectDefBase : public IGameObjectDef
  {
private:
  uint id;
  string nameUnique;
  string nameDisplay;
  string iconFilepath;

public:
  virtual string getUniqueName() const override { return nameUnique; }
  virtual string getDisplayName() const override { return nameDisplay; }
  virtual uint getID() const override { return id; }
  virtual string getIconFilePath() const override { return iconFilepath; }
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  };
