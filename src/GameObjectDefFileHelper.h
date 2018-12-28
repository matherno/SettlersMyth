#pragma once

#include <iostream>
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
#define OD_RENDERPART "renderpart"
#define OD_MESHFILE "meshfile"
#define OD_VOXELFILE "voxelfile"
#define OD_OUTPUT "output"
#define OD_INPUT "input"
#define OD_LOTENTRY "lotentry"
#define OD_BUILDINGENTRY "buildingentry"
#define OD_CONSTRUCTION "construction"
#define OD_AMOUNT "amount"
#define OD_TIME "time"
#define OD_HARVESTER "harvester"
#define OD_ICON "icon"
#define OD_IMAGEFILE "imagefile"
#define OD_UNIT "unit"
#define OD_SPEED "speed"
#define OD_DEPOSIT "deposit"
#define OD_RESOURCENAME "resource_name"
#define OD_RESOURCEAMOUNT "resource_amount"
#define OD_HARVESTTIME "harvest_time"
#define OD_PROCESSTIME "process_time"
#define OD_PICKUP "pickup"
#define OD_MANUFACTURER "manufacturer"
#define OD_MANUF_SPOT "manufacturespot"


static std::string xmlGetStringAttribute(tinyxml2::XMLElement* element, const std::string& attributeName)
  {
  const char* attribute;
  element->QueryStringAttribute(attributeName.c_str(), &attribute);
  if (attribute)
    return std::string(attribute);
  return "";
  }
