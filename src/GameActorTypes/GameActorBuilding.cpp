//
// Created by matt on 27/02/19.
//

#include "GameActorBuilding.h"
#include "SaveLoadFileHelper.h"
#include "BlueprintFileHelper.h"
#include "SMGameContext.h"

/*
*   GameActorBuildingBlueprint
*/

bool GameActorBuildingBlueprint::loadFromXML(XMLElement* xmlElement, string* errorMsg)
  {
  if(!SMGameActorBlueprint::loadFromXML(xmlElement, errorMsg))
    return false;

  XMLElement* xmlGrid = xmlElement->FirstChildElement(OD_GRID);
  if (xmlGrid)
    {
    entryCell = xmlGetGridXYValue(xmlGrid, OD_BUILDINGENTRY);
    XMLElement* xmlResStack = xmlGrid->FirstChildElement(OD_RESSTACK);
    while (xmlResStack)
      {
      int x = xmlResStack->IntAttribute(OD_X, 0);
      int y = xmlResStack->IntAttribute(OD_Y, 0);
      resourceStackSpots.emplace_back(x, y);
      xmlResStack = xmlResStack->NextSiblingElement(OD_RESSTACK);
      }
    }

  return true;
  }
  
bool GameActorBuildingBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  if(!SMGameActorBlueprint::finaliseLoading(gameContext, errorMsg))
    return false;

  return true;
  }

SMGameActor* GameActorBuildingBlueprint::doConstructActor(uint actorID) const
  {
  GameActorBuilding* building = new GameActorBuilding(actorID, id, this);
  building->setupStackCount(resourceStackSpots.size());
  return building;
  }



/*
*   GameActorBuilding
*/

GameActorBuilding::GameActorBuilding(uint id, uint typeID, const GameActorBuildingBlueprint* blueprint) : SMGameActor(id, typeID), blueprint(blueprint)
  {
  }

void GameActorBuilding::onAttached(GameContext* gameContext)
  {
  SMGameActor::onAttached(gameContext);
  returnIdleUnitsTimer.setTimeOut(SEND_SETTLER_COOLDOWN);
  returnIdleUnitsTimer.reset();
  }

void GameActorBuilding::onUpdate(GameContext* gameContext)
  {
  SMGameActor::onUpdate(gameContext);
  if (returnIdleUnitsTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    for (GameActorUnitPtr unit : *(attachedUnits.getList()))
      {
      if (unit->isIdling() && unit->getPosition() != getEntryPosition().centre())
        {
        unit->dropAllResources(gameContext, unit->getPosition());
        unit->returnToAttachedBuilding(gameContext);
        }
      }
    returnIdleUnitsTimer.reset();
    }  
  }

GridXY GameActorBuilding::getEntryPosition() const
  {
  return blueprint->entryCell + getGridPosition();
  }

void GameActorBuilding::attachUnit(GameActorUnitPtr unit)
  {
  attachedUnits.add(unit, unit->getID());
  unit->setAttachedBuilding(getID());
  }

void GameActorBuilding::dettachUnit(uint unitID)
  {
  if (isUnitAttached(unitID))
    {
    if(auto unit = attachedUnits.get(unitID))
      unit->detachFromBuilding();
    attachedUnits.remove(unitID);
    }
  }

void GameActorBuilding::dettachAllUnits()
  {
  for (auto& unit : *attachedUnits.getList())
    unit->detachFromBuilding();
  attachedUnits.clear();
  }

GameActorUnitPtr GameActorBuilding::getIdleUnit()
  {
  for (auto& unit : *attachedUnits.getList())
    {
    if(unit->isIdling())
      return unit;
    }
  return nullptr;
  }

uint GameActorBuilding::getIdleUnitCount()
  {
  uint count = 0;
  for (auto& unit : *attachedUnits.getList())
    {
    if(unit->isIdling())
      count++;
    }
  return count;
  }

GameActorUnitPtr GameActorBuilding::getAttachedUnit(uint unitID)
  {
  if (isUnitAttached(unitID))
    {
    if(auto unit = attachedUnits.get(unitID))
      return unit;
    }
  return nullptr;
  }

bool GameActorBuilding::isUnitAttached(uint unitID) const
  {
  return attachedUnits.contains(unitID);
  }

void GameActorBuilding::getResourceStackPositions(std::vector<GridXY>* positions) const
  {
  const GridXY gridPos = getGridPosition();
  for (GridXY spot : blueprint->resourceStackSpots)
    positions->push_back(gridPos + spot);
  }

void GameActorBuilding::saveActor(XMLElement* element, GameContext* gameContext)
  {
  SMGameActor::saveActor(element, gameContext);

  XMLElement* xmlAttachedUnits = xmlCreateElement(element, SL_ATTACHEDUNITS);
  for (SMGameActorPtr unit : *attachedUnits.getList())
  xmlCreateElement(xmlAttachedUnits, SL_LINKID, unit->getLinkID());
  }


void GameActorBuilding::finaliseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  SMGameActor::finaliseActorFromSave(gameContext, element);
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  XMLElement* xmlAttachedUnits = element->FirstChildElement(SL_ATTACHEDUNITS);
  if (!xmlAttachedUnits)
    return;

  XMLElement* xmlUnitLinkID = xmlAttachedUnits->FirstChildElement(SL_LINKID);
  while (xmlUnitLinkID)
    {
    const uint unitLinkID = (uint) xmlUnitLinkID->IntText(0);
    SMGameActorPtr linkedActor = smGameContext->getSMGameActorByLinkID(unitLinkID);
    if (GameActorUnitPtr unit = std::dynamic_pointer_cast<GameActorUnit>(linkedActor))
      attachUnit(unit);
    xmlUnitLinkID = xmlUnitLinkID->NextSiblingElement(SL_LINKID);
    }
  }