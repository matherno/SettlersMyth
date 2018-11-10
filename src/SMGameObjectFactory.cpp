//
// Created by matt on 8/11/18.
//

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


void GameObjectFactory::forEachGameObjectDef(GameObjectType type, std::function<void(IGameObjectDefPtr)> func) const
  {
  auto heirarchyIter  = std::find(typeHeirarchy.begin(), typeHeirarchy.end(), type);
  if (heirarchyIter == typeHeirarchy.end())
    return;   // type isn't in the typeHeirarchy structure

  //  iterate though each child type, if this type has any
  heirarchyIter++;
  if (heirarchyIter != typeHeirarchy.end() && *heirarchyIter == GameObjectType::LEVELSTART)
    {
    for (heirarchyIter++; heirarchyIter != typeHeirarchy.end() && *heirarchyIter != GameObjectType::LEVELEND; heirarchyIter++)
      forEachGameObjectDef(*heirarchyIter, func);
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

  clearError();
  if (!finaliseGameObjectDefs())
    {
    mathernogl::logError("Failed to finalise Game Object Defs : " + getError());
    return false;
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


bool GameObjectFactory::finaliseGameObjectDefs()
  {
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

    mathernogl::logInfo("Loaded game object def: [" + std::to_string(gameObjectDef->getID()) + "] " + gameObjectDef->getName());
    return true;
    }
  else
    return false;
  }


IGameObjectDef* GameObjectFactory::constructGameObjectDef(string type)
  {
  if (type == "Harvester")
    return new BuildingHarvesterDef();
  return nullptr;
  }

SMGameActorPtr GameObjectFactory::createGameActor(GameContext* gameContext, uint gameObjectDefID) const
  {
  auto gameObjectDef = getGameObjectDef(gameObjectDefID);
  if (gameObjectDef)
    {
    auto actor = gameObjectDef->createGameActor(gameContext);
    if (actor)
      gameContext->addActor(actor);
    return actor;
    }
  ASSERT(false, "Not a valid game object def ID: " + std::to_string(gameObjectDefID));
  return nullptr;
  }

