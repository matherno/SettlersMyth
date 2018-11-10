#pragma once

#include "tinyxml2/tinyxml2.h"

#define GAME_OBJ_DEF_FILE_EXT "smd"

#define OD_GAME_OBJECT_DEF "gameobjectdef"
#define OD_TYPE "type"
#define OD_NAME "name"
#define OD_GRID "grid"
#define OD_X "x"
#define OD_Y "y"
#define OD_XDIR "xdir"
#define OD_YDIR "ydir"
#define OD_RENDER "render"
#define OD_MESHFILE "meshfile"
#define OD_OUTPUT "output"
#define OD_INPUT "input"
#define OD_LOTENTRY "lotentry"
#define OD_BUILDINGENTRY "buildingentry"
#define OD_CONSTRUCTION "construction"
#define OD_RESOURCE "resource"
#define OD_AMOUNT "amount"
#define OD_TIME "time"
#define OD_HARVESTER "harvester"
#define OD_ICON "icon"
#define OD_IMAGEFILE "imagefile"

static string xmlGetStringAttribute(tinyxml2::XMLElement* element, const string& attributeName)
  {
  const char* attribute;
  element->QueryStringAttribute(attributeName.c_str(), &attribute);
  if (attribute)
    return string(attribute);
  return "";
  }
