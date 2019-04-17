//
// Created by matt on 8/11/18.
//

#include <GameActorTypes/GameActorObstacle.h>
#include <GameActorTypes/GameActorBuilding.h>
#include <GameActorTypes/GameActorUnit.h>
#include <GameActorTypes/GameActorResource.h>
#include <GameActorTypes/GameActorDeposit.h>
#include <Components/ComponentVoxelModel.h>
#include <Components/ComponentBuildingVoxelModel.h>
#include <Components/ComponentHarvester.h>
#include <Components/ComponentManufacturer.h>
#include <Components/ComponentStorage.h>
#include <Components/ComponentResourceStacks.h>
#include <Components/ComponentResourcePickup.h>
#include <Components/ComponentDestroyWhenNoResource.h>
#include <Components/ComponentConstructor.h>
#include <Components/ComponentInitResources.h>
#include <Components/ComponentResidents.h>
#include <Components/ComponentUnitRecruiter.h>
#include "SMGameObjectFactory.h"
#include "BlueprintFileHelper.h"
#include "Resources.h"
#include "SMGameContext.h"




/*
 * SMGameActorBlueprint
 */

bool SMGameActorBlueprint::loadFromXML(XMLElement* xmlElement, string* errorMsg)
  {
  name = xmlGetStringAttribute(xmlElement, OD_NAME);
  displayName = xmlGetStringAttribute(xmlElement, OD_DISPLAYNAME);
  description = xmlGetStringAttribute(xmlElement, OD_DESCRIPTION);
  if (name.empty())
    {
    *errorMsg = "Blueprint name is blank";
    return false;
    }
  if (displayName.empty())
    displayName = name;

  auto xmlIcon = xmlElement->FirstChildElement(OD_ICON);
  if (xmlIcon)
    iconPath = RES_DIR + xmlGetStringAttribute(xmlIcon, OD_IMAGEFILE);

  isSelectable = xmlElement->BoolAttribute(OD_SELECTABLE, true);
  isBuildable = xmlElement->BoolAttribute(OD_BUILDABLE, false);

  XMLElement* xmlGrid = xmlElement->FirstChildElement(OD_GRID);
  if (!xmlGrid)
    {
    *errorMsg = "Missing 'grid' element";
    return false;
    }

  gridSize.x = xmlGrid->DoubleAttribute(OD_X, 1);
  gridSize.y = xmlGrid->DoubleAttribute(OD_Y, 1);
  height = xmlGrid->DoubleAttribute(OD_HEIGHT, 2);
  isPosInCentre = xmlGrid->BoolAttribute(OD_POSINCENTRE, false);

  //  clear cells (non-obstacle) cells
  XMLElement* xmlClearCell = xmlGrid->FirstChildElement(OD_CLEARPOS);
  while (xmlClearCell)
    {
    int x = xmlClearCell->IntAttribute(OD_X, 0);
    int y = xmlClearCell->IntAttribute(OD_Y, 0);
    clearGridCells.emplace_back(x, y);
    xmlClearCell = xmlClearCell->NextSiblingElement(OD_CLEARPOS);
    }

  //  load components
  XMLElement* xmlComponent = xmlElement->FirstChildElement(OD_COMPONENT);
  while (xmlComponent)
    {
    if (!loadComponentFromXML(xmlComponent, errorMsg))
      return false;
    xmlComponent = xmlComponent->NextSiblingElement(OD_COMPONENT);
    }

  return true;
  }

bool SMGameActorBlueprint::loadComponentFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  const string componentType = xmlGetStringAttribute(xmlComponent, OD_TYPE);
  const uint componentID = xmlComponent->UnsignedAttribute(OD_ID);

  //  search though existing component blueprints to find if we already have a component with this id
  for (SMComponentBlueprintPtr compBlueprint : componentBlueprints)
    {
    if (compBlueprint->componentID == componentID)
      {
      *errorMsg = "Duplicate component id: '" + std::to_string(componentID) + "', type: '" + componentType + "'";
      return false;
      }
    }

  //  try to construct component blueprint of given type name
  SMComponentBlueprintPtr componentBlueprint = constructComponentBlueprint(componentType);
  if (!componentBlueprint)
    {
    *errorMsg = "Unrecognised component type '" + componentType + "'";
    return false;
    }

  //  load component blueprint parameters
  componentBlueprint->componentID = componentID;
  if (componentBlueprint->loadFromXML(xmlComponent, errorMsg))
    {
    componentBlueprints.push_back(componentBlueprint);
    return true;
    }

  return false;
  }

bool SMGameActorBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  for (auto& componentBlueprint : componentBlueprints)
    {
    if(!componentBlueprint->finaliseLoading(gameContext, errorMsg))
      return false;
    }
  return true;
  }


SMComponentBlueprintPtr SMGameActorBlueprint::constructComponentBlueprint(const string& componentName)
  {
  SMComponentType type = SMTypes::getComponentFromName(componentName);
  if (type == SMComponentType::unknown)
    return nullptr;

  SMComponentBlueprintPtr blueprint;
  switch (type)
    {
    case SMComponentType::voxelModelRenderable:
      blueprint.reset(new ComponentVoxelModelBlueprint());
      break;
    case SMComponentType::buildingVoxelModelRenderable:
      blueprint.reset(new ComponentBuildingVoxelModelBlueprint());
      break;
    case SMComponentType::buildingHarvester:
      blueprint.reset(new ComponentHarvesterBlueprint());
      break;
    case SMComponentType::buildingManufacturer:
      blueprint.reset(new ComponentManufacturerBlueprint());
      break;
    case SMComponentType::buildingStorage:
      blueprint.reset(new ComponentStorageBlueprint());
      break;
    case SMComponentType::resourceStacks:
      blueprint.reset(new ComponentResourceStacksBlueprint());
      break;
    case SMComponentType::resourcePickup:
      blueprint.reset(new ComponentResourcePickupBlueprint());
      break;
    case SMComponentType::destroyWhenNoResource:
      blueprint.reset(new ComponentDestroyWhenNoResourceBlueprint());
      break;
    case SMComponentType::buildingConstructor:
      blueprint.reset(new ComponentConstructorBlueprint());
      break;
    case SMComponentType::initResources:
      blueprint.reset(new ComponentInitResourcesBlueprint());
      break;
    case SMComponentType::buildingResidents:
      blueprint.reset(new ComponentResidentsBlueprint());
      break;
    case SMComponentType::unitRecruiter:
      blueprint.reset(new ComponentUnitRecruiterBlueprint());
      break;
    }

  if (blueprint)
    blueprint->type = type;

  return blueprint;
  }

SMGameActorPtr SMGameActorBlueprint::constructGameActor(uint actorID) const
  {
  SMGameActorPtr actor(doConstructActor(actorID));
  if (actor)
    {
    actor->setHeight(height);
    actor->setSize2D(gridSize);
    actor->setIsPosInCentre(isPosInCentre);
    actor->setTypeName(name);
    actor->setSelectable(isSelectable);

    for (const SMComponentBlueprintPtr& blueprint : componentBlueprints)
      {
      SMComponentPtr component = blueprint->constructComponent(actor);
      if (component)
        {
        component->setComponentID(blueprint->componentID);
        actor->addComponent(component);
        }
      }
    }
  return actor;
  }


/*
 * GameObjectFactory
 */

bool GameObjectFactory::loadBlueprints(GameContext* gameContext, const string& blueprintsDirectory)
  {
  mathernogl::logInfo("Loading game actor blueprint files... ");
  blueprintsMap.clear();

  std::list<string> files;
  mathernogl::getFilesInDirectory(blueprintsDirectory, &files, BLUEPRINT_FILE_EXT);
  for (auto file : files)
    {
    clearError();
    if (!loadBlueprintsFile(blueprintsDirectory + "/" + file))
      {
      mathernogl::logError("Failed to load game actor blueprints : " + getError());
      return false;
      }
    }

  mathernogl::logInfo("Finalising game actor blueprints... ");

  for (auto pair : blueprintsMap)
    {
    if(!pair.second->finaliseLoading(gameContext, &errorMessage))
      {
      mathernogl::logError("Failed to finalise game actor blueprint : '" + pair.second->name + "', " + getError());
      return false;
      }
    }

  return true;
  }


using namespace tinyxml2;

bool GameObjectFactory::loadBlueprintsFile(const string& filePath)
  {
  XMLDocument doc;
  XMLError xmlError = doc.LoadFile(filePath.c_str());
  if (xmlError > 0)
    {
    errorMessage = "Failed to load XML file '" + filePath + "' : " + XMLDocument::ErrorIDToName(xmlError);
    return false;
    }

  XMLElement* xmlBlueprint = doc.FirstChildElement(OD_BLUEPRINT);
  while (xmlBlueprint)
    {
    if (!loadBlueprint(xmlBlueprint))
      {
      errorMessage = "File '" + filePath + "' : " + errorMessage;
      return false;
      }
    xmlBlueprint = xmlBlueprint->NextSiblingElement(OD_BLUEPRINT);
    }

  return true;
  }

bool GameObjectFactory::loadBlueprint(tinyxml2::XMLElement* xmlBlueprint)
  {
  const string blueprintType = xmlGetStringAttribute(xmlBlueprint, OD_TYPE);
  SMGameActorBlueprint* blueprint = constructGameActorBlueprint(blueprintType);
  if (!blueprint)
    {
    errorMessage = "Unrecognised game actor type: '" + blueprintType + "'";
    return false;
    }

  if(blueprint->loadFromXML(xmlBlueprint, &errorMessage))
    {
    mathernogl::logInfo("Loaded game actor blueprint: [" + std::to_string(blueprint->id) + "] " + blueprint->displayName);
    return true;
    }
  else
    {
    errorMessage = "Blueprint '" + blueprint->name + "', " + errorMessage;
    return false;
    }
  }


SMGameActorBlueprint* GameObjectFactory::constructGameActorBlueprint(string typeName)
  {
  SMGameActorType type = SMTypes::getGameActorTypeFromName(typeName);
  if (type == SMGameActorType::unknown)
    return nullptr;

  SMGameActorBlueprintPtr blueprint;
  switch (type)
    {
    case SMGameActorType::obstacle:
      blueprint.reset(new GameActorObstacleBlueprint());
      break;
    case SMGameActorType::building:
      blueprint.reset(new GameActorBuildingBlueprint());
      break;
    case SMGameActorType::unit:
      blueprint.reset(new GameActorUnitBlueprint());
      break;
    case SMGameActorType::resource:
      blueprint.reset(new GameActorResourceBlueprint());
      break;
    case SMGameActorType::deposit:
      blueprint.reset(new GameActorDepositBlueprint());
      break;
    }

  if (blueprint)
    {
    blueprint->id = nextBlueprintID++;
    blueprint->type = type;
    blueprintsMap[blueprint->id] = blueprint;
    return blueprint.get();
    }
  return nullptr;
  }

SMGameActorPtr GameObjectFactory::createGameActor(uint actorID, uint blueprintID)
  {
  auto iter = blueprintsMap.find(blueprintID);
  if (iter == blueprintsMap.end())
    return nullptr;
  SMGameActorBlueprintPtr blueprint = iter->second;

  SMGameActorPtr actor = blueprint->constructGameActor(actorID);
  if (actor)
    {
    while (usedLinkIDs.find(nextLinkID) != usedLinkIDs.end())
      ++nextLinkID;
    actor->setLinkID(nextLinkID);
    usedLinkIDs.insert(nextLinkID);
    ++nextLinkID;
    }
  return actor;
  }

SMGameActorPtr GameObjectFactory::createGameActor(uint actorID, XMLElement* xmlGameActor)
  {
  const string blueprintName = xmlGetStringAttribute(xmlGameActor, SL_TYPE);
  const SMGameActorBlueprint* blueprint = findGameActorBlueprint(blueprintName);
  if (blueprint)
    {
    SMGameActorPtr actor = blueprint->constructGameActor(actorID);
    if (actor)
      actor->setXMLToLoadFrom(xmlGameActor);
    return actor;
    }
  ASSERT(false, "Unknown game actor object definition name + '" + blueprintName + "'!");
  return nullptr;
  }

void GameObjectFactory::freeLinkID(uint linkID)
  {
  usedLinkIDs.erase(linkID);
  }

void GameObjectFactory::registerLinkID(uint linkID)
  {
  ASSERT(usedLinkIDs.count(linkID) == 0, "Duplicate link IDs!");
  usedLinkIDs.insert(linkID);
  }

const SMGameActorBlueprint* GameObjectFactory::getGameActorBlueprint(uint typeID) const
  {
  auto iter = blueprintsMap.find(typeID);
  if (iter != blueprintsMap.end())
    return iter->second.get();
  return nullptr;
  }

const SMGameActorBlueprint* GameObjectFactory::findGameActorBlueprint(string name) const
  {
  for (auto& pair : blueprintsMap)
    {
    if (pair.second->name == name)
      return pair.second.get();
    }
  return nullptr;
  }

void GameObjectFactory::forEachGameActorBlueprint(std::function<void(const SMGameActorBlueprint* blueprint)> func) const
  {
  for (auto& pair : blueprintsMap)
    func(pair.second.get());
  }


