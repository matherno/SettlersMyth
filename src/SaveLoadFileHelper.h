#pragma once
//
// Created by matt on 24/11/18.
//


#include <tinyxml2/tinyxml2.h>
#include "Grid.h"
#include "MathernoGL.h"

#define SAVES_DIRECTORY "saves/"
#define SAVES_FILE_EXT "sms"
#define MAX_SAVE_NAME_LENGTH  30
#define SAVE_CURRENT_VERSION 1.0


#define SL_SMSAVE  "sm_save_file"
#define SL_VERSION  "version"

#define SL_X "x"
#define SL_Y "y"
#define SL_Z "z"
#define SL_R "r"
#define SL_G "g"
#define SL_B "b"
#define SL_LINKID "linkid"
#define SL_BEHAVIOUR "behaviour"
#define SL_TYPE "type"
#define SL_SMGAMEACTOR "smgameactor"
#define SL_GRIDPOS "gridpos"
#define SL_CELLPOS "cellpos"
#define SL_POS "pos"
#define SL_ROT "rot"
#define SL_ELEV "elevation"
#define SL_RESOURCELIST "resource_list"
#define SL_RESOURCE "resource"
#define SL_NAME "name"
#define SL_AMOUNT "amount"
#define SL_MODELIDX "modelidx"
#define SL_ATTACHEDUNITS "attached_units"
#define SL_RESIDENTUNITS "resident_units"
#define SL_COMPONENT "component"
#define SL_ID "id"
#define SL_UNDER_CONSTR "under_constr"
#define SL_CONSTR_RESOURCELIST "constr_resource_list"
#define SL_RESOURCELIST "resource_list"
#define SL_COLLECT "collect"

#define SL_CAMERA_POS  "cam_pos"
#define SL_CAMERA_ZOOM  "cam_zoom"
#define SL_CAMERA_ROT  "cam_rot"
#define SL_CAMERA_PITCH  "cam_pitch"


typedef tinyxml2::XMLElement XMLElement;
typedef tinyxml2::XMLDocument XMLDocument;
typedef tinyxml2::XMLError XMLError;

static string xmlToString(double val, int decimalPlaces = 2)
  {
  string format = mathernogl::stringFormat("%%.%df", decimalPlaces);
  return mathernogl::stringFormat(format, val);
  }

static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, string name)
  {
  tinyxml2::XMLElement* element = doc.NewElement(name.c_str());
  if (parent)
    parent->InsertEndChild(element);
  else
    doc.InsertEndChild(element);
  return element;
  }

static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, string name, string value)
  {
  auto element = xmlCreateElement(doc, parent, name);
  element->SetText(value.c_str());
  return element;
  }

static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, string name, double value)
  {
  xmlCreateElement(doc, parent, name, xmlToString(value));
  }

static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, string name, Vector3D value)
  {
  auto element = xmlCreateElement(doc, parent, name);
  element->SetAttribute(SL_X, xmlToString(value.x).c_str());
  element->SetAttribute(SL_Y, xmlToString(value.y).c_str());
  element->SetAttribute(SL_Z, xmlToString(value.z).c_str());
  return element;
  }

static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, string name, Vector2D value)
  {
  auto element = xmlCreateElement(doc, parent, name);
  element->SetAttribute(SL_X, xmlToString(value.x).c_str());
  element->SetAttribute(SL_Y, xmlToString(value.y).c_str());
  return element;
  }

static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, string name, GridXY value)
  {
  auto element = xmlCreateElement(doc, parent, name);
  element->SetAttribute(SL_X, xmlToString(value.x).c_str());
  element->SetAttribute(SL_Y, xmlToString(value.y).c_str());
  return element;
  }

static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLElement* parent, string name)
  {
  return xmlCreateElement(*parent->GetDocument(), parent, name);
  }

template<typename T> static tinyxml2::XMLElement* xmlCreateElement(tinyxml2::XMLElement* parent, string name, T value)
  {
  return xmlCreateElement(*parent->GetDocument(), parent, name, value);
  }

static double xmlGetDblValue(tinyxml2::XMLElement* parent, string name)
  {
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    return element->DoubleText();
  return 0;
  }

static Vector3D xmlGetVec3Value(tinyxml2::XMLElement* parent, string name)
  {
  Vector3D value;
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    {
    element->QueryAttribute(SL_X, &value.x);
    element->QueryAttribute(SL_Y, &value.y);
    element->QueryAttribute(SL_Z, &value.z);
    }
  return value;
  }

static Vector3D xmlGetVec3Value(tinyxml2::XMLElement* element)
  {
  Vector3D value;
  if (element)
    {
    element->QueryAttribute(SL_X, &value.x);
    element->QueryAttribute(SL_Y, &value.y);
    element->QueryAttribute(SL_Z, &value.z);
    }
  return value;
  }

static Vector2D xmlGetVec2Value(tinyxml2::XMLElement* parent, string name)
  {
  Vector2D value;
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    {
    element->QueryAttribute(SL_X, &value.x);
    element->QueryAttribute(SL_Y, &value.y);
    }
  return value;
  }

static GridXY xmlGetGridXYValue(tinyxml2::XMLElement* parent, string name)
  {
  GridXY value;
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    {
    element->QueryAttribute(SL_X, &value.x);
    element->QueryAttribute(SL_Y, &value.y);
    }
  return value;
  }

static int xmlGetGridIntValue(tinyxml2::XMLElement* parent, string name)
  {
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    return element->IntText();
  return 0;
  }

static bool xmlGetBoolValue(tinyxml2::XMLElement* parent, string name)
  {
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    return element->BoolText();
  return 0;
  }

static string xmlGetValue(tinyxml2::XMLElement* parent, string name)
  {
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    return element->GetText();
  return "";
  }

static string xmlConstructCompElemName(uint id)
  {
  return string(SL_COMPONENT) + "_" + std::to_string(id);
  }

static Vector3D xmlGetRGBValue(tinyxml2::XMLElement* parent, string name)
  {
  Vector3D value(1, 0, 0);
  auto element = parent->FirstChildElement(name.c_str());
  if (element)
    {
    element->QueryAttribute(SL_R, &value.x);
    element->QueryAttribute(SL_G, &value.y);
    element->QueryAttribute(SL_B, &value.z);
    value.x /= 255;
    value.y /= 255;
    value.z /= 255;
    }
  return value;
  }