//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentResidents.h"
#include "UIResourceAmount.h"
#include "UISystem/UIText.h"


/*
*   ComponentResidentsBlueprint
*/

bool ComponentResidentsBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  baseTimeBetweenUnitGen = xmlComponent->IntAttribute(OD_UNITGENTIME, 5 * 60) * 1000;
  maxUnits = xmlComponent->IntAttribute(OD_MAXUNITS, 10);
  initUnits = xmlComponent->IntAttribute(OD_INITUNITS, 0);
  unitBlueprintName = xmlGetStringAttribute(xmlComponent, OD_UNIT);
  return true;
  }

bool ComponentResidentsBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  const SMGameActorBlueprint* unitBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(unitBlueprintName);
  if (!unitBlueprint)
    {
    *errorMsg = "Unit name '" + unitBlueprintName + "' unknown.";
    return false;
    }
  unitBlueprintID = unitBlueprint->id;
  return true;
  }

SMComponentPtr ComponentResidentsBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentResidents(actor, type, this));
  }




/*
*   ComponentResidents
*/

ComponentResidents::ComponentResidents(const SMGameActorPtr& actor, SMComponentType type, const ComponentResidentsBlueprint* blueprint) : BuildingComponent(actor, type), blueprint(blueprint)
  {
  generateUnits = blueprint->baseTimeBetweenUnitGen > 0;
  unitGenRate = 10;
  if (generateUnits)
    {
    unitGenTimer.setTimeOut(blueprint->baseTimeBetweenUnitGen);
    unitGenTimer.reset();
    }
  }

void ComponentResidents::initialise(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  if (!building->getIsUnderConstruction())
    createInitialResidents(gameContext);
  }

void ComponentResidents::initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent)
  {
  }

void ComponentResidents::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  BuildingComponent::onMessage(gameContext, message, extra);

  switch (message)
    {
    case SMMessage::constructionFinished:
      createInitialResidents(gameContext);
      break;
    }
  }

void ComponentResidents::update(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  if (building->getIsUnderConstruction())
    return;

  if (!generateUnits || building->getResidentUnitCount() >= blueprint->maxUnits)
    {
    unitGenTimer.reset();
    return;
    }

  if (unitGenTimer.incrementTimer(gameContext->getDeltaTime() * unitGenRate))
    {
    createResident(gameContext);
    unitGenTimer.reset();
    }
  }

void ComponentResidents::cleanUp(GameContext* gameContext)
  {
  
  }

void ComponentResidents::createInitialResidents(GameContext* gameContext)
  {
  for(int i = 0; i < blueprint->initUnits; ++i)
    createResident(gameContext);
  }

void ComponentResidents::createResident(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  SMGameActorPtr actor = SMGameContext::cast(gameContext)->createSMGameActor(blueprint->unitBlueprintID, building->getEntryPosition());
  GameActorUnitPtr unit = std::dynamic_pointer_cast<GameActorUnit>(actor);
  building->addResidentUnit(unit);
  }

int ComponentResidents::onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset)
  {
  GameActorBuilding* building = getBuildingActor();
  const SMGameActorBlueprint* unitBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->getGameActorBlueprint(blueprint->unitBlueprintID);
  if (!unitBlueprint)
    return 0;

  int thisHUDHeight = 0;

  uiOccupancyCount.reset(new UIResourceAmount(gameContext->getUIManager()->getNextComponentID(), unitBlueprint->iconPath, building->getResidentUnitCount()));
  uiOccupancyCount->setOffset(Vector2D(0, yOffset));
  uiOccupancyCount->setHorizontalAlignment(alignmentCentre);
  parentPanel->addChild(uiOccupancyCount);
  thisHUDHeight = uiOccupancyCount->getSize().y;

  if (generateUnits)
    {
    const int unitGenTextPadding = 10;
    uiUnitGenText.reset(new UIText(gameContext->getUIManager()->getNextComponentID()));
    uiUnitGenText->setOffset(Vector2D(0, yOffset + thisHUDHeight + unitGenTextPadding));
    uiUnitGenText->setSize(Vector2D(0, 30));
    uiUnitGenText->setFontSize(20);
    uiUnitGenText->setHorizontalAlignment(alignmentCentre);
    uiUnitGenText->setWidthMatchParent(true);
    uiUnitGenText->setPadding(10, 10);
    uiUnitGenText->setTextCentreAligned(true, true);
    uiUnitGenText->showBackground(false);
    parentPanel->addChild(uiUnitGenText);
    thisHUDHeight += uiUnitGenText->getSize().y + unitGenTextPadding;
    }

  return thisHUDHeight;
  }

void ComponentResidents::onUpdateSelectionHUD(GameContext* gameContext) 
  {
  GameActorBuilding* building = getBuildingActor();
  if (uiOccupancyCount)
    uiOccupancyCount->updateAmount(building->getResidentUnitCount());

  if (uiUnitGenText)
    {
    double percentToNextUnit = (double) unitGenTimer.getTimeRemaining() / (double) blueprint->baseTimeBetweenUnitGen;
    percentToNextUnit = 1.0 - percentToNextUnit;
    percentToNextUnit *= 100;

    uiUnitGenText->setText(blueprint->unitBlueprintName + " generation: " + mathernogl::stringFormat("%.0f%%", percentToNextUnit));
    uiUnitGenText->invalidate();
    }
  }
