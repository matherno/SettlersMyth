#pragma once

#include <iostream>
#include "tinyxml2/tinyxml2.h"

#define BLUEPRINT_FILE_EXT "smd"

#define OD_BLUEPRINT "blueprint"
#define OD_COMPONENT "component"
#define OD_TYPE "type"
#define OD_ID "id"
#define OD_NAME "name"
#define OD_DISPLAYNAME "displayname"
#define OD_DESCRIPTION "description"
#define OD_GRID "grid"
#define OD_X "x"
#define OD_Y "y"
#define OD_Z "z"
#define OD_HEIGHT "height"
#define OD_ELEV "elev"
#define OD_ROT "rot"
#define OD_XDIR "xdir"
#define OD_YDIR "ydir"
#define OD_RENDER "render"
#define OD_MODEL "model"
#define OD_MESHFILE "meshfile"
#define OD_VOXELFILE "voxelfile"
#define OD_OUTPUT "output"
#define OD_INPUT "input"
#define OD_RESSTACK "resourcestack"
#define OD_STACKPOS "stackpos"
#define OD_CLEARPOS "clearpos"
#define OD_BUILDINGENTRY "entry"
#define OD_CONSTRUCTION "construction"
#define OD_AMOUNT "amount"
#define OD_TIME "time"
#define OD_HARVESTER "harvester"
#define OD_ICON "icon"
#define OD_IMAGEFILE "imagefile"
#define OD_UNIT "unit"
#define OD_MAXUNITS "max_units"
#define OD_SPEED "speed"
#define OD_DEPOSIT "deposit"
#define OD_RESOURCENAME "resource_name"
#define OD_RESOURCEAMOUNT "resource_amount"
#define OD_HARVESTTIME "harvest_time"
#define OD_PROCESSTIME "process_time"
#define OD_PICKUP "pickup"
#define OD_MANUFACTURER "manufacturer"
#define OD_MANUF_SPOT "manufacturespot"
#define OD_MANUF_DIR "manufacturedir"
#define OD_RESOURCE "resource"
#define OD_STATICMODEL "staticmodel"
#define OD_POSINCENTRE "posincentre"
#define OD_SELECTABLE "selectable"
#define OD_BUILDABLE "buildable"
#define OD_CONSTRUCTIONPACK "construction_pack"
#define OD_CONSTRUCTIONTIME "construction_time"
#define OD_KEEPOUTPUT "keepoutput"


static std::string xmlGetStringAttribute(tinyxml2::XMLElement* element, const std::string& attributeName)
  {
  const char* attribute = nullptr;
  element->QueryStringAttribute(attributeName.c_str(), &attribute);
  if (attribute)
    return std::string(attribute);
  return "";
  }
