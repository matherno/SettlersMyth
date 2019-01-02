//
// Created by matt on 10/11/18.
//

#include <Building.h>
#include "BuildingDef.h"
#include "GameObjectDefFileHelper.h"

bool BuildingDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!StaticObjectDef::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlGrid = xmlGameObjectDef->FirstChildElement(OD_GRID);
  if (!xmlGrid)
    {
    *errorMsg = "Building missing 'grid' element";
    return false;
    }

  //  output spots
  auto xmlOutput = xmlGrid->FirstChildElement(OD_OUTPUT);
  while (xmlOutput)
    {
    int x = xmlOutput->IntAttribute(OD_X, 0);
    int y = xmlOutput->IntAttribute(OD_Y, 0);
    outputSpots.emplace_back(x, y);
    xmlOutput = xmlOutput->NextSiblingElement(OD_OUTPUT);
    }

  //  input spots
  auto xmlInput = xmlGrid->FirstChildElement(OD_INPUT);
  while (xmlInput)
    {
    int x = xmlInput->IntAttribute(OD_X, 0);
    int y = xmlInput->IntAttribute(OD_Y, 0);
    inputSpots.emplace_back(x, y);
    xmlInput = xmlInput->NextSiblingElement(OD_INPUT);
    }

  //  building entry
  auto xmlBuildingEntry = xmlGrid->FirstChildElement(OD_BUILDINGENTRY);
  if (xmlBuildingEntry)
    {
    buildingEntry.x = xmlBuildingEntry->IntAttribute(OD_X, 0);
    buildingEntry.y = xmlBuildingEntry->IntAttribute(OD_Y, 0);
    }
  else
    {
    *errorMsg = "No building entry definition found";
    return false;
    }

  //  construction
  auto xmlConstruction = xmlGameObjectDef->FirstChildElement(OD_CONSTRUCTION);
  if (xmlConstruction)
    {
    constructionTime = xmlConstruction->IntAttribute(OD_TIME);
    auto xmlResource = xmlConstruction->FirstChildElement(OD_RESOURCENAME);
    while (xmlResource)
      {
      const string resourceName = xmlGetStringAttribute(xmlResource, OD_NAME);
      const int resourceAmount = xmlResource->IntAttribute(OD_AMOUNT, 1);
      constructionReq.emplace_back(resourceName, resourceAmount);
      xmlResource = xmlResource->NextSiblingElement(OD_RESOURCENAME);
      }
    }


  return true;
  }

SMGameActorPtr BuildingDef::createGameActor(GameContext* gameContext) const
  {
  Building* actor = new Building(gameContext->getNextActorID(), this);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }
