#pragma once

#include "SMGameActor.h"
#include "Unit.h"

/*
*   
*/

class Building : public SMStaticActor
  {
private:
  mathernogl::MappedList<UnitPtr> attachedUnits;

public:
  Building(uint id, const IGameObjectDef* gameObjectDef);

  void attachUnit(UnitPtr unit);
  void dettachUnit(uint unitID);
  void dettachAllUnits();
  UnitPtr getIdleUnit();
  uint getIdleUnitCount();
  UnitPtr getAttachedUnit(uint unitID);
  const mathernogl::MappedList<UnitPtr>* getAttachedUnits() const { return &attachedUnits; }
  bool isUnitAttached(uint unitID) const;

  void returnIdleUnits(SMGameActor* gameActor, GameContext* gameContext);

  static Building* cast(SMGameActor* gameActor){ return dynamic_cast<Building*>(gameActor); }
  };
