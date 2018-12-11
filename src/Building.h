#pragma once

#include "SMGameActor.h"
#include "Unit.h"

/*
*   
*/

class Building : public SMStaticActor
  {
private:
  mathernogl::MappedList<UnitWkPtr> attachedUnits;

public:
  Building(uint id, const IGameObjectDef* gameObjectDef);

  void attachUnit(UnitPtr unit);
  void dettachUnit(uint unitID);
  void dettachAllUnits();
  Unit* getIdleUnit();
  Unit* getAttachedUnit(uint unitID);
  const mathernogl::MappedList<UnitWkPtr>* getAttachedUnits() const { return &attachedUnits; }
  bool isUnitAttached(uint unitID) const;

  static Building* cast(SMGameActor* gameActor){ return dynamic_cast<Building*>(gameActor); }
  };
