//
// Created by matt on 8/11/18.
//

#include <GameObjectDefs/ResourceDepositDef.h>
#include <GameObjectDefs/UnitDef.h>
#include <GameObjectDefs/ResourceDef.h>
#include <GameObjectDefs/BuildingManufacturerDef.h>
#include <GameObjectDefs/BuildingStorageDef.h>
#include "SMGameObjectFactory.h"
#include "GameObjectDefFileHelper.h"
#include "GameObjectDefs/BuildingHarvesterDef.h"


void GameObjectFactory::getGameObjectDefs(GameObjectType type, std::vector<IGameObjectDefPtr>* defsList) const
  {
  forEachGameObjectDef(type, [defsList](const IGameObjectDefPtr def)
    {
    defsList->push_back(def);
    });
  }


IGameObjectDefPtr GameObjectFactory::getGameObjectDef(uint id) const
  {
  if (gameObjectDefs.count(id) > 0)
    return gameObjectDefs.at(id);
  return nullptr;
  }

IGameObjectDefPtr GameObjectFactory::findGameObjectDef(string name) const
  {
  for (auto pair : gameObjectDefs)
    {
    if (pair.second->getUniqueName() == name)
      return pair.second;
    }
  return nullptr;
  }

uint GameObjectFactory::findGameObjectDefID(string name) const
  {
  if (auto def = findGameObjectDef(name))
    return def->getID();
  return 0;
  }

void GameObjectFactory::forEachGameObjectDef(GameObjectType type, std::function<void(IGameObjectDefPtr)> func) const
  {
  ASSERT(type != GameObjectType::LEVELSTART && type != GameObjectType::LEVELEND, "");
  auto hierarchyIter = std::find(typeHierarchy.begin(), typeHierarchy.end(), type);
  if (hierarchyIter == typeHierarchy.end())
    return;   // type isn't in the typeHeirarchy structure
  forEachGameObjectDef(hierarchyIter, func);
  }


void GameObjectFactory::forEachGameObjectDef(std::list<GameObjectType>::const_iterator& hierarchyIter, std::function<void(IGameObjectDefPtr)> func) const
  {
  const GameObjectType type = *hierarchyIter;

  //  iterate though each child type, if this type has any
  hierarchyIter++;
  if (hierarchyIter != typeHierarchy.end() && *hierarchyIter == GameObjectType::LEVELSTART)
    {
    hierarchyIter++;
    while (hierarchyIter != typeHierarchy.end() && *hierarchyIter != GameObjectType::LEVELEND)
      {
      forEachGameObjectDef(hierarchyIter, func);
      hierarchyIter++;
      }
    }
  else
    {
    hierarchyIter--;
    }

  //  iterate through each game object def of this type if any
  if (typesToGameDefs.count(type) == 0)
    return;
  for (uint defID : typesToGameDefs.at(type))
    {
    auto def = getGameObjectDef(defID);
    if (def) func(def);
    }
  }


bool GameObjectFactory::loadGameObjectDefs(const string& defsDirectory)
  {
  mathernogl::logInfo("Loading game object def files... ");
  clearGameObjectDefs();

  std::list<string> files;
  mathernogl::getFilesInDirectory(defsDirectory, &files, GAME_OBJ_DEF_FILE_EXT);
  for (auto file : files)
    {
    clearError();
    if (!loadGameObjectDefFile(defsDirectory + "/" + file))
      {
      mathernogl::logError("Failed to load Game Object Defs : " + getError());
      return false;
      }
    }
  return true;
  }


void GameObjectFactory::clearGameObjectDefs()
  {
  typesToGameDefs.clear();
  gameObjectDefs.clear();
  }


using namespace tinyxml2;

bool GameObjectFactory::loadGameObjectDefFile(const string& filePath)
  {
  XMLDocument doc;
  XMLError xmlError = doc.LoadFile(filePath.c_str());
  if (xmlError > 0)
    {
    errorMessage = "Failed to load XML file '" + filePath + "' : " + XMLDocument::ErrorIDToName(xmlError);
    return false;
    }

  XMLElement* xmlGameObjectDef = doc.FirstChildElement(OD_GAME_OBJECT_DEF);
  while (xmlGameObjectDef)
    {
    if (!loadGameObjectDef(xmlGameObjectDef))
      {
      errorMessage = "File '" + filePath + "' : " + errorMessage;
      return false;
      }
    xmlGameObjectDef = xmlGameObjectDef->NextSiblingElement(OD_GAME_OBJECT_DEF);
    }

  return true;
  }

bool GameObjectFactory::loadGameObjectDef(tinyxml2::XMLElement* xmlGameObjectDef)
  {
  const string gameObjType = xmlGetStringAttribute(xmlGameObjectDef, OD_TYPE);
  IGameObjectDef* gameObjectDef = constructGameObjectDef(gameObjType);
  if (!gameObjectDef)
    {
    errorMessage = "Unrecognised game object type: '" + gameObjType + "'";
    return false;
    }

  if(gameObjectDef->loadFromXML(xmlGameObjectDef, &errorMessage))
    {
    const uint id = gameObjectDef->getID();
    const GameObjectType type = gameObjectDef->getType();

    gameObjectDefs[id] = std::shared_ptr<IGameObjectDef>(gameObjectDef);
    if(typesToGameDefs.count(type) == 0)
      typesToGameDefs[type] = std::vector<uint>();
    typesToGameDefs[type].push_back(id);

    mathernogl::logInfo("Loaded game object def: [" + std::to_string(gameObjectDef->getID()) + "] " + gameObjectDef->getDisplayName());
    return true;
    }
  else
    return false;
  }


IGameObjectDef* GameObjectFactory::constructGameObjectDef(string name)
  {
  GameObjectType type = getTypeBasedOnTypeName(name);
  if (type == GameObjectType::harvester)
    return new BuildingHarvesterDef();
  if (type == GameObjectType::manufacturer)
    return new BuildingManufacturerDef();
  if (type == GameObjectType::storage)
    return new BuildingStorageDef();

  if (type == GameObjectType::deposit)
    return new ResourceDepositDef();

  if (isTypeOrSubType(GameObjectType::unit, type))
    return new UnitDef();

  if (type == GameObjectType::resource)
    return new ResourceDef();

  if (isTypeOrSubType(GameObjectType::staticObject, type))
    return new ObstacleDef();
  return nullptr;
  }

SMGameActorPtr GameObjectFactory::createGameActor(GameContext* gameContext, uint gameObjectDefID, const Vector2D& position)
  {
  auto gameObjectDef = getGameObjectDef(gameObjectDefID);
  if (gameObjectDef)
    {
    SMGameActorPtr actor = gameObjectDef->createGameActor(gameContext);
    if (actor)
      {
      auto staticActor = SMStaticActor::cast(actor.get());
      if (staticActor)
        staticActor->setGridPos(position);
      auto dynamicActor = SMDynamicActor::cast(actor.get());
      if (dynamicActor)
        dynamicActor->setPosition(position);
      gameContext->addActor(actor);
      }

    while (usedLinkIDs.count(nextLinkID) > 0)
      ++nextLinkID;
    actor->setLinkID(nextLinkID);
    usedLinkIDs.insert(nextLinkID);
    ++nextLinkID;
    return actor;
    }

  ASSERT(false, "Not a valid game object def ID: " + std::to_string(gameObjectDefID));
  return nullptr;
  }

SMGameActorPtr GameObjectFactory::createGameActor(GameContext* gameContext, XMLElement* xmlGameActor)
  {
  string sObjDefName = xmlGetStringAttribute(xmlGameActor, SL_GAMEOBJDEF_NAME);
  auto gameObjectDef = findGameObjectDef(sObjDefName);
  if (gameObjectDef)
    {
    auto actor = gameObjectDef->createGameActor(gameContext);
    if (actor)
      {
      actor->setXMLToLoadFrom(xmlGameActor);
      gameContext->addActor(actor);
      usedLinkIDs.insert(actor->getLinkID());
      }
    return actor;
    }
  ASSERT(false, "Unknown game actor object definition name");
  return nullptr;
  }

bool GameObjectFactory::isSubType(GameObjectType type, GameObjectType subType) const
  {
  auto iter = std::find(typeHierarchy.begin(), typeHierarchy.end(), type);
  int numLevels = 0;
  do
    {
    ++iter;
    if (*iter == subType)
      return true;
    else if (*iter == GameObjectType::LEVELSTART)
      ++numLevels;
    else if (*iter == GameObjectType::LEVELEND)
      --numLevels;
    }
  while (numLevels >= 1);
  return false;
  }

bool GameObjectFactory::isTypeOrSubType(GameObjectType type, GameObjectType subType) const
  {
  return type == subType || isSubType(type, subType);
  }

string GameObjectFactory::getTypeName(GameObjectType type)
  {
  if (typeNames.count(type) > 0)
    return typeNames.at(type);
  ASSERT(false, "");
  return "";
  }

GameObjectType GameObjectFactory::getTypeBasedOnTypeName(string typeName)
  {
  for (auto pair : typeNames)
    {
    if (pair.second.compare(typeName) == 0)
      return pair.first;
    }
  return GameObjectType::none;
  }

void GameObjectFactory::freeLinkID(uint linkID)
  {
  usedLinkIDs.erase(linkID);
  }

