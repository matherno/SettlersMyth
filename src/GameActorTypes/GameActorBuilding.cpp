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

  XMLElement* xmlConstruction = xmlElement->FirstChildElement(OD_CONSTRUCTION);
  if(xmlConstruction)
    {
    constructionPackName = xmlGetStringAttribute(xmlConstruction, OD_CONSTRUCTIONPACK);
    xmlConstruction->QueryAttribute(OD_AMOUNT, &constructionPackAmount);
    }

  return true;
  }
  
bool GameActorBuildingBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  if(!SMGameActorBlueprint::finaliseLoading(gameContext, errorMsg))
    return false;

  constructionPackID = 0;
  if (!constructionPackName.empty())
    {
    const SMGameActorBlueprint* resourceBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(constructionPackName);
    if (!resourceBlueprint)
      {
      *errorMsg = "Construction pack name '" + constructionPackName + "' unknown.";
      return false;
      }
    constructionPackID = resourceBlueprint->id;
    }

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
  if(blueprint->constructionPackID > 0 && blueprint->constructionPackAmount > 0)
    {
    isUnderConstruction = true;
    constructionResourceStorage.setupStackCount(1, blueprint->constructionPackAmount);
    constructionResourceStorage.setResourceObserverFunc([this, gameContext]()
      {
      makeConstructed(gameContext);     //  checks internally if can be constructed or not
      if (getIsUnderConstruction())
        postMessage(gameContext, SMMessage::constructionProgress);
      });
    }
  else
    {
    isUnderConstruction = false;
    }

  SMGameActor::onAttached(gameContext);
  returnIdleUnitsTimer.setTimeOut(SEND_SETTLER_COOLDOWN);
  returnIdleUnitsTimer.reset();
  }

void GameActorBuilding::onUpdate(GameContext* gameContext)
  {
  SMGameActor::onUpdate(gameContext);

  if (!isUnderConstruction)
    onUpdateReturnIdleUnits(gameContext);
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

void GameActorBuilding::addResidentUnit(GameActorUnitPtr unit)
  {
  ASSERT(unit->getResidentsBuilding() == 0 || unit->getResidentsBuilding() == getID(), "Unit already residented to a building?");
  residentUnits.add(unit, unit->getID());
  unit->setResidentsBuilding(getID());
  }

void GameActorBuilding::removeResidentUnit(uint unitID)
  {
  if (GameActorUnitPtr unit = residentUnits.get(unitID))
    {
    ASSERT(unit->getResidentsBuilding() == getID(), "Unit not residented to this building?");
    residentUnits.remove(unitID);
    unit->setResidentsBuilding(0);
    }
  }

GameActorUnitPtr GameActorBuilding::getDettachedResidentUnit(uint unitBlueprintTypeID)
  {
  for (GameActorUnitPtr unit : *(residentUnits.getList()))
    {
    if (!unit->isAttachedToBuilding())
      {
      if (unitBlueprintTypeID == 0)
        return unit;
      else if (unit->getBlueprintTypeID() == unitBlueprintTypeID)
        return unit;
      }
    }
  return nullptr;
  }

uint GameActorBuilding::getResidentUnitCount() const
  {
  return residentUnits.count();
  }

uint GameActorBuilding::getDettachedResidentUnitCount() const
  {
  uint count = 0;
  for (GameActorUnitPtr unit : *(residentUnits.getList()))
    {
    if (!unit->isAttachedToBuilding())
      count++;
    }
  return count;
  }

void GameActorBuilding::saveActor(XMLElement* element, GameContext* gameContext)
  {
  SMGameActor::saveActor(element, gameContext);

  XMLElement* xmlAttachedUnits = xmlCreateElement(element, SL_ATTACHEDUNITS);
  for (SMGameActorPtr unit : *attachedUnits.getList())
    xmlCreateElement(xmlAttachedUnits, SL_LINKID, unit->getLinkID());

  XMLElement* xmlResidentUnits = xmlCreateElement(element, SL_RESIDENTUNITS);
  for (SMGameActorPtr unit : *residentUnits.getList())
    xmlCreateElement(xmlResidentUnits, SL_LINKID, unit->getLinkID());

  xmlCreateElement(element, SL_UNDER_CONSTR, isUnderConstruction);
  
  if (isUnderConstruction)
    {
    //  save construction resources
    SMGameContext* smGameContext = SMGameContext::cast(gameContext);
    XMLElement* resListElem = xmlCreateElement(element, SL_CONSTR_RESOURCELIST);
    for (const ResourceStack& stack : *constructionResourceStorage.getResourceStacks())
      {
      const SMGameActorBlueprint* resourceBlueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(stack.id);
      if (resourceBlueprint)
        {
        XMLElement* resourceElem = xmlCreateElement(resListElem, SL_RESOURCE);
        resourceElem->SetAttribute(SL_NAME, resourceBlueprint->name.c_str());
        resourceElem->SetAttribute(SL_AMOUNT, stack.amount);
        }
      }
    }
  }

void GameActorBuilding::initialiseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  isUnderConstruction = xmlGetBoolValue(element, SL_UNDER_CONSTR);
  if (isUnderConstruction)
    {
    XMLElement* resListElem = element->FirstChildElement(SL_CONSTR_RESOURCELIST);
    if (resListElem)
      {
      SMGameContext* smGameContext = SMGameContext::cast(gameContext);
      XMLElement* resourceElem = resListElem->FirstChildElement(SL_RESOURCE);
      while (resourceElem)
        {
        const string name = xmlGetStringAttribute(resourceElem, SL_NAME);
        int amount = 0;
        resourceElem->QueryAttribute(SL_AMOUNT, &amount);
        const SMGameActorBlueprint* resourceBlueprint = smGameContext->getGameObjectFactory()->findGameActorBlueprint(name);
        if (resourceBlueprint && resourceBlueprint->type == SMGameActorType::resource)
          constructionResourceStorage.storeResource(resourceBlueprint->id, amount);
        else
          mathernogl::logWarning("Loading Actor: Couldn't find resource of name: " + name);
        resourceElem = resourceElem->NextSiblingElement(SL_RESOURCE);
        }
      }
    }

  SMGameActor::initialiseActorFromSave(gameContext, element);
  }


void GameActorBuilding::finaliseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  SMGameActor::finaliseActorFromSave(gameContext, element);
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  //  attached units
  XMLElement* xmlAttachedUnits = element->FirstChildElement(SL_ATTACHEDUNITS);
  if (xmlAttachedUnits)
    {
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

  // resident units
  XMLElement* xmlResidentUnits = element->FirstChildElement(SL_RESIDENTUNITS);
  if (xmlResidentUnits)
    {
    XMLElement* xmlUnitLinkID = xmlResidentUnits->FirstChildElement(SL_LINKID);
    while (xmlUnitLinkID)
      {
      const uint unitLinkID = (uint) xmlUnitLinkID->IntText(0);
      SMGameActorPtr linkedActor = smGameContext->getSMGameActorByLinkID(unitLinkID);
      if (GameActorUnitPtr unit = std::dynamic_pointer_cast<GameActorUnit>(linkedActor))
        addResidentUnit(unit);
      xmlUnitLinkID = xmlUnitLinkID->NextSiblingElement(SL_LINKID);
      }
    }
  }

bool GameActorBuilding::getIsUnderConstruction() const
  {
  return isUnderConstruction;
  }

ResourceStorage* GameActorBuilding::getConstructionResourceStorage()
  {
  return &constructionResourceStorage;
  }

void GameActorBuilding::makeConstructed(GameContext* gameContext, bool force)
  {
  if (force || constructionResourceStorage.resourceCount(blueprint->constructionPackID) >= blueprint->constructionPackAmount)
    {
    isUnderConstruction = false;
    constructionResourceStorage.clearAllResources();
    postMessage(gameContext, SMMessage::constructionFinished);
    }
  }

uint GameActorBuilding::getConstructionPackID() const
  {
  return blueprint->constructionPackID;
  }

double GameActorBuilding::getConstructionProgress() const
  {
  const double progress = (double)constructionResourceStorage.resourceCount(blueprint->constructionPackID) / (double)blueprint->constructionPackAmount;
  return mathernogl::clampd(progress, 0, 1);
  }

void GameActorBuilding::onUpdateReturnIdleUnits(GameContext* gameContext)
  {
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

int GameActorBuilding::onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset)
  {
  UIManager* uiManager = gameContext->getUIManager();

  // temp force construction finished button
  UIButton* button = new UIButton(uiManager->getNextComponentID(), false);
  button->setOffset(Vector2D(10, yOffset));
  button->setSize(Vector2D(30, 30));
  button->setButtonColour(Vector3D(0.7, 0.1, 0.1));
  button->setButtonHighlightColour(BTN_PRESSED_COL, BTN_UNPRESSED_COL);
  button->setHighlightWidth(BTN_BORDER_SIZE);
  button->setMouseClickCallback([this, gameContext](uint x, uint y) -> bool
    {
    makeConstructed(gameContext, true);
    return true;
    });
  parentPanel->addChild(UIComponentPtr(button));

  return 30;
  }