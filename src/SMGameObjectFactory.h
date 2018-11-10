#pragma once

#include <TowOff/RenderSystem/RenderSystem.h>
#include <TowOff/GameSystem/GameSystem.h>
#include "tinyxml2/tinyxml2.h"
#include "SMGameActor.h"


/*
*   
*/


enum class GameObjectType
  {
  none,
  all,

  staticObject,
  deposit,
  building,
  harvester,
  manufacturer,
  storage,
  obstacle,

  pickup,
  resource,

  LEVELSTART,   // special codes for the hierarchy
  LEVELEND,
  };


static const std::list<GameObjectType> typeHeirarchy
  {
  GameObjectType::all,
    GameObjectType::LEVELSTART,

    GameObjectType::staticObject,
      GameObjectType::LEVELSTART,
      GameObjectType::deposit,
      GameObjectType::building,
        GameObjectType::LEVELSTART,
        GameObjectType::harvester,
        GameObjectType::manufacturer,
        GameObjectType::storage,
        GameObjectType::LEVELEND,
      GameObjectType::obstacle,
      GameObjectType::LEVELEND,

    GameObjectType::pickup,
      GameObjectType::LEVELSTART,
      GameObjectType::resource,
      GameObjectType::LEVELEND,

    GameObjectType::LEVELEND,
  };

class SMGameActor;
typedef std::shared_ptr<SMGameActor> SMGameActorPtr;

class IGameObjectBehaviour
  {
public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) = 0;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) = 0;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) = 0;
  };
typedef std::shared_ptr<IGameObjectBehaviour> IGameObjectBehaviourPtr;


class IGameObjectDef
  {
public:
  virtual string getName() const = 0;
  virtual uint getID() const = 0;
  virtual string getIconFilePath() const = 0;
  virtual GameObjectType getType() const = 0;
  virtual RenderablePtr constructRenderable(RenderContext* renderContext) const = 0;

private:
  friend class GameObjectFactory;
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) = 0;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const = 0;
  };
typedef std::shared_ptr<const IGameObjectDef> IGameObjectDefPtr;


class GameObjectFactory
  {
private:
  std::map<uint, std::shared_ptr<const IGameObjectDef>> gameObjectDefs;
  std::map<GameObjectType, std::vector<uint>> typesToGameDefs;
  string errorMessage;

public:
  GameObjectFactory() {};

  void getGameObjectDefs(GameObjectType type, std::vector<IGameObjectDefPtr>* defsList) const;
  IGameObjectDefPtr getGameObjectDef(uint id) const;
  void forEachGameObjectDef(GameObjectType type, std::function<void(IGameObjectDefPtr)> func) const;
  SMGameActorPtr createGameActor(GameContext* gameContext, uint gameObjectDefID) const;

  bool loadGameObjectDefs(const string& defsDirectory);
  string getError() const { return errorMessage; }
  void clearError() { errorMessage = ""; }

private:
  void clearGameObjectDefs();
  bool loadGameObjectDefFile(const string& filePath);
  bool loadGameObjectDef(tinyxml2::XMLElement* xmlGameObjectDef);
  bool finaliseGameObjectDefs();
  IGameObjectDef* constructGameObjectDef(string type);
  };
