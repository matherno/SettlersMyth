//
// Created by matt on 4/12/18.
//

#include "Building.h"
#include "SMGameContext.h"

Building::Building(uint id, const IGameObjectDef* gameObjectDef) : SMStaticActor(id, gameObjectDef)
  {}

void Building::saveActor(XMLElement* element, GameContext* gameContext)
  {
  SMStaticActor::saveActor(element, gameContext);
  XMLElement* xmlAttachedUnits = xmlCreateElement(element, SL_ATTACHEDUNITS);
  for (SMGameActorPtr unit : *attachedUnits.getList())
    xmlCreateElement(xmlAttachedUnits, SL_LINKID, unit->getLinkID());
  }

void Building::finaliseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  SMGameActor::finaliseActorFromSave(gameContext, element);
  XMLElement* xmlAttachedUnits = element->FirstChildElement(SL_ATTACHEDUNITS);
  if (!xmlAttachedUnits)
    return;

  XMLElement* xmlUnitLinkID = xmlAttachedUnits->FirstChildElement(SL_LINKID);
  while (xmlUnitLinkID)
    {
    const uint unitLinkID = (uint) xmlUnitLinkID->IntText(0);
    SMGameActorPtr linkedActor = smGameContext->getSMGameActorByLinkID(unitLinkID);
    if (UnitPtr unit = Unit::cast(linkedActor))
      attachUnit(unit);
    xmlUnitLinkID = xmlUnitLinkID->NextSiblingElement(SL_LINKID);
    }
  }

void Building::attachUnit(UnitPtr unit)
  {
  attachedUnits.add(unit, unit->getID());
  unit->setAttachedBuilding(getID());
  }

void Building::dettachUnit(uint unitID)
  {
  if (isUnitAttached(unitID))
    {
    if(auto unit = attachedUnits.get(unitID))
      unit->detachFromBuilding();
    attachedUnits.remove(unitID);
    }
  }

void Building::dettachAllUnits()
  {
  for (auto& unit : *attachedUnits.getList())
    unit->detachFromBuilding();
  attachedUnits.clear();
  }

UnitPtr Building::getIdleUnit()
  {
  for (auto& unit : *attachedUnits.getList())
    {
    if(unit->isIdling())
      return unit;
    }
  return nullptr;
  }

uint Building::getIdleUnitCount()
  {
  uint count = 0;
  for (auto& unit : *attachedUnits.getList())
    {
    if(unit->isIdling())
      count++;
    }
  return count;
  }

UnitPtr Building::getAttachedUnit(uint unitID)
  {
  if (isUnitAttached(unitID))
    {
    if(auto unit = attachedUnits.get(unitID))
      return unit;
    }
  return nullptr;
  }

bool Building::isUnitAttached(uint unitID) const
  {
  return attachedUnits.contains(unitID);
  }

void Building::returnIdleUnits(SMGameActor* gameActor, GameContext* gameContext)
  {
  Building* building = Building::cast(gameActor);
  for (auto unit : *building->getAttachedUnits()->getList())
    {
    if(unit->isIdling() && building->getGridPosition() != GridXY(unit->getPosition()))
      unit->processCommand(CMD_RETURN_TO_BASE, gameContext);
    }
  }

