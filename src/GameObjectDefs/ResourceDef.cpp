//
// Created by matt on 9/12/18.
//

#include <GameObjectDefFileHelper.h>
#include "ResourceDef.h"


bool ResourceDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!DynamicObjectDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  //  resource stack positions
  stackPosAndRots.clear();
  auto xmlStackPos = xmlGameObjectDef->FirstChildElement(OD_STACKPOS);
  while (xmlStackPos)
    {
    const double x = xmlStackPos->DoubleAttribute(OD_X, 0);
    const double y = xmlStackPos->DoubleAttribute(OD_ELEV, 0);
    const double z = -xmlStackPos->DoubleAttribute(OD_Y, 0);
    const double rot = xmlStackPos->DoubleAttribute(OD_ROT, 0);
    stackPosAndRots.emplace_back(Vector3D(x, y, z), rot);
    xmlStackPos = xmlStackPos->NextSiblingElement(OD_STACKPOS);
    }

  return true;
  }

