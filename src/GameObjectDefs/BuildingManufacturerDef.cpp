//
// Created by matt on 16/12/18.
//

#include <GameObjectDefFileHelper.h>
#include <Behaviours/ManufacturerBehaviour.h>
#include <Behaviours/ResourceStackBehaviour.h>
#include "BuildingManufacturerDef.h"


bool BuildingManufacturerDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!BuildingDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  if (XMLElement* xmlManuf = xmlGameObjectDef->FirstChildElement(OD_MANUFACTURER))
    {
    xmlManuf->QueryAttribute(OD_PROCESSTIME, &manufactureTime);
    manufactureSpot = xmlGetGridXYValue(xmlManuf, OD_MANUF_SPOT);
    manufactureDir = xmlGetGridXYValue(xmlManuf, OD_MANUF_DIR);
    clearGridCells.emplace_back(manufactureSpot);

    //  inputs
    int inputResCount = 0;
    XMLElement* xmlInput = xmlManuf->FirstChildElement(OD_INPUT);
    while(xmlInput)
      {
      string resName = xmlGetStringAttribute(xmlInput, OD_NAME);
      uint resAmnt = 0;
      xmlInput->QueryAttribute(OD_AMOUNT, &resAmnt);
      if (resAmnt > 0 && !resName.empty())
        {
        inputs[resName] = resAmnt;
        inputResCount += resAmnt;
        }
      xmlInput = xmlInput->NextSiblingElement(OD_INPUT);
      }

    //  output
    XMLElement* xmlOutput = xmlManuf->FirstChildElement(OD_OUTPUT);
    if(xmlOutput)
      {
      outputResName = xmlGetStringAttribute(xmlOutput, OD_NAME);
      outputResAmount = 0;
      xmlOutput->QueryAttribute(OD_AMOUNT, &outputResAmount);
      }

    if (outputResName.empty() || outputResAmount == 0 || inputResCount == 0)
      {
      *errorMsg = "Invalid manufacturer input/output config";
      return false;
      }
    }
  else
    {
    *errorMsg = "No manufacturer definition found";
    return false;
    }

  return true;
  }

void BuildingManufacturerDef::createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const
  {
  IGameObjectDef::createActorBehaviours(behaviourList);
  behaviourList->push_back(IGameObjectBehaviourPtr(new ManufacturerBehaviour()));
  behaviourList->push_back(IGameObjectBehaviourPtr(new ResourceStackBehaviour()));
  }

