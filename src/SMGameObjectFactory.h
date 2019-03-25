#pragma once

#include <RenderSystem/RenderSystem.h>
#include <GameSystem/GameSystem.h>
#include <set>
#include "tinyxml2/tinyxml2.h"
#include "SMGameActor.h"
#include "SaveLoadFileHelper.h"
#include "Utils.h"



class SMComponentBlueprint
  {
public:
  SMComponentType type;
  uint componentID;
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) = 0;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) { return true; }
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const = 0;
  };
typedef std::shared_ptr<SMComponentBlueprint> SMComponentBlueprintPtr;

class SMGameActorBlueprint
  {
public:
  uint id = 0;
  string name;
  string displayName;
  string iconPath;
  SMGameActorType type;
  Vector2D gridSize;
  double height = 1;
  bool isPosInCentre = false;
  bool isSelectable = false;
  bool isBuildable = false;
  std::vector<GridXY> clearGridCells;    //  cells that units can move through
  std::vector<SMComponentBlueprintPtr> componentBlueprints;

  virtual bool loadFromXML(XMLElement* xmlElement, string* errorMsg);
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg);
  virtual SMGameActorPtr constructGameActor(uint actorID) const;

protected:
  virtual SMGameActor* doConstructActor(uint actorID) const = 0;

private:
  bool loadComponentFromXML(XMLElement* xmlComponent, string* errorMsg);
  SMComponentBlueprintPtr constructComponentBlueprint(const string& componentName);
  };
typedef std::shared_ptr<SMGameActorBlueprint> SMGameActorBlueprintPtr;


class GameObjectFactory
  {
private:
  std::map<uint, SMGameActorBlueprintPtr> blueprintsMap;
  string errorMessage;
  std::set<uint> usedLinkIDs;
  uint nextLinkID = 1;
  uint nextBlueprintID = 1;

public:
  GameObjectFactory() {};

  bool loadBlueprints(GameContext* gameContext, const string& blueprintsDirectory);
  string getError() const { return errorMessage; }
  void clearError() { errorMessage = ""; }

  SMGameActorPtr createGameActor(uint actorID, uint blueprintID);
  SMGameActorPtr createGameActor(uint actorID, XMLElement* xmlGameActor);
  void freeLinkID(uint linkID);
  void registerLinkID(uint linkID);
  const SMGameActorBlueprint* getGameActorBlueprint(uint typeID) const;
  const SMGameActorBlueprint* findGameActorBlueprint(string name) const;
  void forEachGameActorBlueprint(std::function<void(const SMGameActorBlueprint* blueprint)> func) const;

private:
  void clearGameObjectDefs();
  bool loadBlueprintsFile(const string& filePath);
  bool loadBlueprint(tinyxml2::XMLElement* xmlBlueprint);
  SMGameActorBlueprint* constructGameActorBlueprint(string type);
  };
