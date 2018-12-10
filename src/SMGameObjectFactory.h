#pragma once

#include <TowOff/RenderSystem/RenderSystem.h>
#include <TowOff/GameSystem/GameSystem.h>
#include <set>
#include "tinyxml2/tinyxml2.h"
#include "SMGameActor.h"
#include "SaveLoadFileHelper.h"
#include "SMActorCommand.h"


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

  unit,
  settler,
  citizen,
  //  animals?,

  LEVELSTART,   // special codes for the hierarchy
  LEVELEND,
  };


static const std::list<GameObjectType> typeHierarchy
  {
  GameObjectType::all,
    GameObjectType::LEVELSTART,

    GameObjectType::staticObject,
      GameObjectType::LEVELSTART,
      GameObjectType::building,
        GameObjectType::LEVELSTART,
        GameObjectType::harvester,
        GameObjectType::manufacturer,
        GameObjectType::storage,
        GameObjectType::LEVELEND,
      GameObjectType::deposit,
      GameObjectType::obstacle,
      GameObjectType::LEVELEND,

    GameObjectType::pickup,
      GameObjectType::LEVELSTART,
      GameObjectType::resource,
      GameObjectType::LEVELEND,

    GameObjectType::unit,
      GameObjectType::LEVELSTART,
      GameObjectType::settler,
      GameObjectType::citizen,
      GameObjectType::LEVELEND,

    GameObjectType::LEVELEND,
  };

static const std::map<GameObjectType, string> typeNames
  {
    { GameObjectType::LEVELSTART, "LEVEL_START" },
    { GameObjectType::LEVELEND, "LEVEL_END" },
    { GameObjectType::all, "All" },
    { GameObjectType::none, "None" },
    { GameObjectType::staticObject, "Static Object" },
    { GameObjectType::deposit, "Deposit" },
    { GameObjectType::building, "Building" },
    { GameObjectType::harvester, "Harvester" },
    { GameObjectType::manufacturer, "Manufacturer" },
    { GameObjectType::storage, "Storage" },
    { GameObjectType::obstacle, "Obstacle" },
    { GameObjectType::pickup, "Pickup" },
    { GameObjectType::resource, "Resource" },
    { GameObjectType::unit, "Unit" },
    { GameObjectType::settler, "Settler" },
    { GameObjectType::citizen, "Citizen" },
  };

class GameObjectFactory;
class SMGameActor;
typedef std::shared_ptr<SMGameActor> SMGameActorPtr;


class IGameObjectBehaviour
  {
public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) = 0;
  virtual void initialiseFromSaved(SMGameActor* gameActor, GameContext* gameContext, XMLElement* xmlElement) { initialise(gameActor, gameContext); }
  virtual void save(SMGameActor* gameActor, GameContext* gameContext, XMLElement* xmlElement) {};
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) = 0;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) = 0;
  virtual string getBehaviourName() { return ""; };
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) { return false; }  // return true if handled
  };
typedef std::shared_ptr<IGameObjectBehaviour> IGameObjectBehaviourPtr;


class BehaviourHelper : public IGameObjectBehaviour
  {
public:
  typedef std::function<void(SMGameActor*, GameContext*)> Callback;

private:
  Callback onInit, onUpdate;

public:
  BehaviourHelper (Callback fInit, Callback fUpdate) : onInit(fInit), onUpdate(fUpdate) {}
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override { if (onInit) onInit(gameActor, gameContext); };
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override { if (onUpdate) onUpdate(gameActor, gameContext); };
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override {};
  };


class IGameObjectDef
  {
public:
  virtual string getUniqueName() const = 0;
  virtual string getDisplayName() const = 0;
  virtual uint getID() const = 0;
  virtual string getIconFilePath() const = 0;
  virtual GameObjectType getType() const = 0;
  virtual RenderablePtr constructRenderable(RenderContext* renderContext) const = 0;

protected:
  friend class GameObjectFactory;
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) = 0;
  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const = 0;
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const {};
  };
typedef std::shared_ptr<const IGameObjectDef> IGameObjectDefPtr;


class GameObjectFactory
  {
private:
  std::map<uint, IGameObjectDefPtr> gameObjectDefs;
  std::map<GameObjectType, std::vector<uint>> typesToGameDefs;
  string errorMessage;
  std::set<uint> usedLinkIDs;
  uint nextLinkID = 1;

public:
  GameObjectFactory() {};

  void getGameObjectDefs(GameObjectType type, std::vector<IGameObjectDefPtr>* defsList) const;
  IGameObjectDefPtr getGameObjectDef(uint id) const;
  IGameObjectDefPtr findGameObjectDef(string name) const;
  void forEachGameObjectDef(GameObjectType type, std::function<void(IGameObjectDefPtr)> func) const;
  void forEachGameObjectDef(std::list<GameObjectType>::const_iterator& hierarchyIter, std::function<void(IGameObjectDefPtr)> func) const;
  SMGameActorPtr createGameActor(GameContext* gameContext, XMLElement* xmlGameActor);
  SMGameActorPtr createGameActor(GameContext* gameContext, uint gameObjectDefID);
  bool isSubType(GameObjectType type, GameObjectType subType) const;
  bool isTypeOrSubType(GameObjectType type, GameObjectType subType) const;
  void freeLinkID(uint linkID);

  bool loadGameObjectDefs(const string& defsDirectory);
  string getError() const { return errorMessage; }
  void clearError() { errorMessage = ""; }

  static string getTypeName(GameObjectType type);
  static GameObjectType getTypeBasedOnTypeName(string name);

private:
  void clearGameObjectDefs();
  bool loadGameObjectDefFile(const string& filePath);
  bool loadGameObjectDef(tinyxml2::XMLElement* xmlGameObjectDef);
  IGameObjectDef* constructGameObjectDef(string type);
  };

