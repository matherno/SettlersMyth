//
// Created by matt on 10/11/18.
//

#include "GameObjectDefBase.h"
#include "GameObjectDefFileHelper.h"
#include "Resources.h"

static uint nextID = 1;

bool GameObjectDefBase::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  id = nextID;
  nextID++;
  nameUnique = xmlGetStringAttribute(xmlGameObjectDef, OD_NAME);
  if (nameUnique.empty())
    {
    *errorMsg = "Game object def name is blank";
    return false;
    }

  nameDisplay = nameUnique;
  auto xmlIcon = xmlGameObjectDef->FirstChildElement(OD_ICON);
  if (xmlIcon)
    iconFilepath = RES_DIR + xmlGetStringAttribute(xmlIcon, OD_IMAGEFILE);
  return true;
  }

