//
// Created by matt on 10/11/18.
//

#include "BuildingHarvesterDef.h"
#include "GameObjectDefFileHelper.h"

bool BuildingHarvesterDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!BuildingDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlHarvester = xmlGameObjectDef->FirstChildElement(OD_HARVESTER);
  if (xmlHarvester)
    {
    }
  else
    {
    *errorMsg = "No harvester definition found";
    return false;
    }

  return true;
  }

