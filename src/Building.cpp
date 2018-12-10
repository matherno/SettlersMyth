//
// Created by matt on 4/12/18.
//

#include "Building.h"

Building::Building(uint id, const IGameObjectDef* gameObjectDef) : SMStaticActor(id, gameObjectDef)
  {}

void Building::attachUnit(UnitPtr unit)
  {
  attachedUnits.add(unit, unit->getID());
  unit->setAttachedBuilding(getID());
  }

void Building::dettachUnit(uint unitID)
  {
  if (isUnitAttached(unitID))
    {
    if(auto unit = attachedUnits.get(unitID).lock())
      unit->detachFromBuilding();
    attachedUnits.remove(unitID);
    }
  }

void Building::dettachAllUnits()
  {
  for (auto& unitPtr : *attachedUnits.getList())
    {
    if (auto unit = unitPtr.lock())
      unit->detachFromBuilding();
    }
  attachedUnits.clear();
  }

Unit* Building::getIdleUnit()
  {
  for (auto& unitPtr : *attachedUnits.getList())
    {
    if (auto unit = unitPtr.lock())
      {
      if(unit->isIdling())
        return unit.get();
      }
    }
  return nullptr;
  }

Unit* Building::getAttachedUnit(uint unitID)
  {
  if (isUnitAttached(unitID))
    {
    if(auto unit = attachedUnits.get(unitID).lock())
      return unit.get();
    }
  return nullptr;
  }

bool Building::isUnitAttached(uint unitID) const
  {
  return attachedUnits.contains(unitID);
  }
