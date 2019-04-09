#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>
#include <GameActorTypes/GameActorUnit.h>


/*
 *
 */

class GameActorUnit;
typedef std::shared_ptr<GameActorUnit> GameActorUnitPtr;

class GameActorBuildingBlueprint : public SMGameActorBlueprint
  {
private:
  string constructionPackName = "";

public:
  GridXY entryCell;
  std::vector<GridXY> resourceStackSpots;
  uint constructionPackID = 0;
  uint constructionPackAmount = 0;

protected:
  virtual bool loadFromXML(XMLElement* xmlElement, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMGameActor *doConstructActor(uint actorID) const override;
  };

class GameActorBuilding : public SMGameActor
  {
private:
  const GameActorBuildingBlueprint* blueprint;
  mathernogl::MappedList<GameActorUnitPtr> attachedUnits;
  Timer returnIdleUnitsTimer;

  bool isUnderConstruction = false;
  ResourceStorage constructionResourceStorage;

public:
  GameActorBuilding(uint id, uint typeID, const GameActorBuildingBlueprint* blueprint);

  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void saveActor(XMLElement* element, GameContext* gameContext) override;

  GridXY getEntryPosition() const;
  
  void attachUnit(GameActorUnitPtr unit);
  void dettachUnit(uint unitID);
  void dettachAllUnits();
  GameActorUnitPtr getIdleUnit();
  uint getIdleUnitCount();
  GameActorUnitPtr getAttachedUnit(uint unitID);
  const mathernogl::MappedList<GameActorUnitPtr>* getAttachedUnits() const { return &attachedUnits; }
  bool isUnitAttached(uint unitID) const;
  void getResourceStackPositions(std::vector<GridXY>* positions) const;

  bool getIsUnderConstruction() const;
  ResourceStorage* getConstructionResourceStorage();
  void makeConstructed(GameContext* gameContext, bool force = false);
  uint getConstructionPackID() const;
  double getConstructionProgress() const;   //  from 0 to 1

  static GameActorBuilding* cast(SMGameActor* actor) { return dynamic_cast<GameActorBuilding*>(actor); }

protected:
  virtual void finaliseActorFromSave(GameContext* gameContext, XMLElement* element) override;
  void onUpdateReturnIdleUnits(GameContext* gameContext);
  };
