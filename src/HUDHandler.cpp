//
// Created by matt on 28/01/18.
//

#include "HUDHandler.h"
#include "SMGameContext.h"
#include "Resources.h"
#include "Utils.h"
#include "GameActorTypes/GameActorBuilding.h"


/*
 *  ActorFocusPanel
 */

void ActorFocusPanel::initialise(GameContext* context)
  {
  UIPanel::initialise(context);
  UIManager* uiManager = context->getUIManager();

  setColour(HUD_COL_BORDER);

  UIPanel* subPanel = new UIPanel(uiManager->getNextComponentID());
  subPanel->setColour(HUD_COL_BG);
  subPanel->setVerticalAlignment(alignmentCentre);
  subPanel->setHorizontalAlignment(alignmentCentre);
  subPanel->setHeightMatchParent(true);
  subPanel->setWidthMatchParent(true);
  subPanel->setPadding(HUD_BORDER_SIZE, HUD_BORDER_SIZE);
  addChild(UIComponentPtr(subPanel));

  UIPanel* iconBorder = new UIPanel(uiManager->getNextComponentID());
  iconBorder->setColour(BTN_UNPRESSED_COL);
  iconBorder->setOffset(Vector2D(10, 10));
  iconBorder->setSize(Vector2D(45));
  iconBorder->setPadding(HUD_BORDER_SIZE, HUD_BORDER_SIZE);
  addChild(UIComponentPtr(iconBorder));

  icon.reset(new UIPanel(uiManager->getNextComponentID()));
  icon->setVerticalAlignment(alignmentCentre);
  icon->setHorizontalAlignment(alignmentCentre);
  icon->setHeightMatchParent(true);
  icon->setWidthMatchParent(true);
  icon->setPadding(BTN_BORDER_SIZE, BTN_BORDER_SIZE);
  iconBorder->addChild(icon);

  nameText.reset(new UIText(uiManager->getNextComponentID()));
  nameText->setOffset(Vector2D(10, 60));
  nameText->setSize(Vector2D(250, 30));
  nameText->setFontSize(25);
  nameText->setFontColour(Vector3D(0));
  nameText->showBackground(false);
  subPanel->addChild(nameText);

  attachedUnitText.reset(new UIText(uiManager->getNextComponentID()));
  attachedUnitText->setOffset(Vector2D(65, 20));
  attachedUnitText->setSize(Vector2D(170, 30));
  attachedUnitText->setFontSize(25);
  attachedUnitText->setFontColour(Vector3D(0));
  attachedUnitText->showBackground(false);
  subPanel->addChild(attachedUnitText);

  resourceText.reset(new UIText(uiManager->getNextComponentID()));
  resourceText->setOffset(Vector2D(0, 90));
  resourceText->setSize(Vector2D(0, 100));
  resourceText->setWidthMatchParent(true);
  resourceText->setPadding(10, 10);
  resourceText->setFontSize(22);
  resourceText->setFontColour(Vector3D(0));
  resourceText->showBackground(false);
  subPanel->addChild(resourceText);

  // temp force construction finished button
  UIButton* button = new UIButton(uiManager->getNextComponentID(), false);
  button->setVerticalAlignment(Alignment::alignmentEnd);
  button->setHorizontalAlignment(Alignment::alignmentEnd);
  button->setOffset(Vector2D(-10, -10));
  button->setSize(Vector2D(30, 30));
  button->setButtonColour(Vector3D(0.7, 0.1, 0.1));
  button->setButtonHighlightColour(BTN_PRESSED_COL, BTN_UNPRESSED_COL);
  button->setHighlightWidth(BTN_BORDER_SIZE);
  button->setMouseClickCallback([this, context](uint x, uint y) -> bool
    {
    GameActorBuilding* buildingActor = GameActorBuilding::cast(focusActor.get());
    if (buildingActor)
      buildingActor->makeConstructed(context, true);
    return true;
    });
  subPanel->addChild(UIComponentPtr(button));
  }

void ActorFocusPanel::updateActorInfo(GameContext* context)
  {
  SMGameContext* smGameContext = SMGameContext::cast(context);

  if (SMGameActorPtr actor = smGameContext->getSelectionManager()->getFirstSelectedActor())
    {
    setVisible(true, true);


    if (!focusActor || focusActor->getID() != actor->getID())
      {
      focusActor = actor;
      const SMGameActorBlueprint* blueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(actor->getBlueprintTypeID());
      ASSERT(blueprint, "");
      nameText->setText(blueprint->displayName);
      nameText->invalidate();
      if (!blueprint->iconPath.empty())
        icon->setTexture(context->getRenderContext()->getSharedTexture(blueprint->iconPath));
      else
        icon->setTexture(nullptr);
      icon->invalidate();
      }

    GameActorBuilding* buildingActor = GameActorBuilding::cast(actor.get());
    bool buildingUnderConstruction = false;
    string unitText;
    if (buildingActor)
      {
      buildingUnderConstruction = buildingActor->getIsUnderConstruction();
      if (!buildingUnderConstruction)
        {
        const int attachedUnitCount = buildingActor->getAttachedUnits()->count();
        const int residentUnitCount = buildingActor->getDettachedResidentUnitCount();
        const int idleCount = buildingActor->getIdleUnitCount();
        unitText = mathernogl::stringFormat("%d|%d|%d", idleCount, attachedUnitCount, residentUnitCount);
        }
      }
    attachedUnitText->setText(unitText);
    attachedUnitText->invalidate();

    bool first = true;
    string resText;
    if (buildingUnderConstruction)
      {
      resText = "Under Construction\n";
      const SMGameActorBlueprint* resourceBlueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(buildingActor->getConstructionPackID());
      if (resourceBlueprint)
        resText += resourceBlueprint->displayName + "\n";
      resText += mathernogl::stringFormat("Progress %.0f%%", buildingActor->getConstructionProgress() * 100.0);
      }
    else
      {
      actor->forEachResource([&](uint id, uint amount)
        {
        const SMGameActorBlueprint* resourceBlueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(id);
        if (resourceBlueprint)
          {
          if (!first)
            resText += "\n";
          resText += resourceBlueprint->displayName + ": " + std::to_string(amount);
          first = false;
          }
        }, true, true);
      }

    resourceText->setText(resText);
    resourceText->invalidate();
    }
  else
    {
    setVisible(false, true);
    focusActor.reset();
    }
  }


/*
 *  SMDebugPanel
 */

void SMDebugPanel::initialise(GameContext* context)
  {
  UIPanel::initialise(context);

  UIManager* uiManager = context->getUIManager();

  textComponent.reset(new UIText(uiManager->getNextComponentID()));
  textComponent->setPadding(10, 10);
  textComponent->setFontSize(20);
  textComponent->setFontColour(Vector3D(0));
  textComponent->showBackground(false);
  textComponent->setFontColour(Vector3D(0.1));
  textComponent->setHeightMatchParent(true);
  textComponent->setWidthMatchParent(true);
  textComponent->setVisible(isVisible(), true);
  addChild(textComponent);

  refreshTimer.setTimeOut(1000);
  }

void SMDebugPanel::updateDebugInfo(GameContext* context)
  {
  if (textComponent && isVisible() && refreshTimer.incrementTimer(context->getDeltaTime()))
    {
    SMGameContext* smGameContext = SMGameContext::cast(context);
    string text;
    text += "Game Time: " + mathernogl::formatTime(context->getGameTime()) + "\n";
    text += "Delta Time: " + std::to_string(smGameContext->getDeltaTime()) + " ms\n";
    text += "FPS: " + std::to_string(int(1.0f / ((float)smGameContext->getDeltaTime() / 1000.0f))) + "\n";
    text += "Speed: " + std::to_string(smGameContext->getSpeed()) + "\n";
    text += "Actors: " + std::to_string(smGameContext->getSMActorCount()) + "\n";

    textComponent->setText(text);
    textComponent->setVisible(isVisible(), true);
    textComponent->invalidate();
    refreshTimer.reset();
    }
  }


/*
 *  HUDHandler
 */

void HUDHandler::initialiseUI(GameContext* context)
  {
  UIManager* uiManager = context->getUIManager();
  mainUIPanel.reset(new UIPanel(uiManager->getNextComponentID()));
  mainUIPanel->setOffset(Vector2D(0, 0));
  mainUIPanel->setSize(Vector2D(160, 0));
  mainUIPanel->setColour(HUD_COL_BORDER);
  mainUIPanel->setHorizontalAlignment(alignmentEnd);
  mainUIPanel->setVerticalAlignment(alignmentEnd);
  mainUIPanel->setHeightMatchParent(true);
  uiManager->addComponent(mainUIPanel);

  setupBuildPanel(context);
  setupFocusPanel(context);
  setupDebugPanel(context);
  }

void HUDHandler::updateUI(GameContext* context)
  {
  focusPanel->updateActorInfo(context);
  debugPanel->updateDebugInfo(context);
  }

void HUDHandler::deselectUI()
  {
  buildingButtonGroup->forceDeselectAll();
  }

void HUDHandler::toggleDebugPanel()
  {
  if (!debugPanel)
    return;
  debugPanel->setVisible(!debugPanel->isVisible(), true);
  }

void HUDHandler::setupFocusPanel(GameContext* context)
  {
  UIManager* uiManager = context->getUIManager();
  focusPanel.reset(new ActorFocusPanel(uiManager->getNextComponentID()));
  focusPanel->setOffset(Vector2D(0, 20));
  focusPanel->setSize(Vector2D(250, 200));
  uiManager->addComponent(focusPanel);
  }

void HUDHandler::setupBuildPanel(GameContext* context)
  {
  UIManager* uiManager = context->getUIManager();
  SMGameContext* smGameContext = SMGameContext::cast(context);

  UIPanel* subPanel = new UIPanel(uiManager->getNextComponentID());
  subPanel->setVerticalAlignment(alignmentCentre);
  subPanel->setHorizontalAlignment(alignmentCentre);
  subPanel->setHeightMatchParent(true);
  subPanel->setWidthMatchParent(true);
  subPanel->setColour(HUD_COL_BG);
  subPanel->setPadding(HUD_BORDER_SIZE, HUD_BORDER_SIZE);
  mainUIPanel->addChild(UIComponentPtr(subPanel));



  /*
  *   Selected building details
  */

  selectedBuildPanel.reset(new UIPanel(uiManager->getNextComponentID()));
  selectedBuildPanel->setVerticalAlignment(alignmentEnd);
  selectedBuildPanel->setHorizontalAlignment(alignmentCentre);
  selectedBuildPanel->setSize(Vector2D(0, 250));
  selectedBuildPanel->setWidthMatchParent(true);
  selectedBuildPanel->setColour(HUD_COL_BG);
  selectedBuildPanel->setPadding(0, 0);
  subPanel->addChild(selectedBuildPanel);

  UIPanel* iconBorder = new UIPanel(uiManager->getNextComponentID());
  iconBorder->setColour(BTN_UNPRESSED_COL);
  iconBorder->setOffset(Vector2D(10, 0));
  iconBorder->setSize(Vector2D(45));
  iconBorder->setPadding(HUD_BORDER_SIZE, HUD_BORDER_SIZE);
  iconBorder->setVisible(false, true);
  selectedBuildPanel->addChild(UIComponentPtr(iconBorder));

  selectedBuildIcon.reset(new UIPanel(uiManager->getNextComponentID()));
  selectedBuildIcon->setVerticalAlignment(alignmentCentre);
  selectedBuildIcon->setHorizontalAlignment(alignmentCentre);
  selectedBuildIcon->setHeightMatchParent(true);
  selectedBuildIcon->setWidthMatchParent(true);
  selectedBuildIcon->setPadding(BTN_BORDER_SIZE, BTN_BORDER_SIZE);
  selectedBuildIcon->setVisible(false, true);
  iconBorder->addChild(selectedBuildIcon);

  selectedBuildText.reset(new UIText(uiManager->getNextComponentID()));
  selectedBuildText->setOffset(Vector2D(0, 50));
  selectedBuildText->setSize(Vector2D(0, 200));
  selectedBuildText->setWidthMatchParent(true);
  selectedBuildText->setPadding(10, 10);
  selectedBuildText->setFontSize(20);
  selectedBuildText->setFontColour(Vector3D(0));
  selectedBuildText->showBackground(false);
  selectedBuildPanel->addChild(selectedBuildText);



  /*
  *   Place building buttons
  */

  static const int numCols = 2;
  static const int btnSize = 45;
  static const int btnHalfSize = int(btnSize * 0.5);
  static const int btnPadding = 13;
  static const int btnHalfPadding = int(btnPadding * 0.5);
  static const int btnSizeAndPadding = btnSize + btnPadding;
  buildingButtonGroup.reset(new UIToggleButtonGroup());

  int buildingNum = 0;
  smGameContext->getGameObjectFactory()->forEachGameActorBlueprint([&](const SMGameActorBlueprint* blueprint) -> void
    {
    if (!blueprint->isBuildable)
      return;

    const int colNum = buildingNum % numCols;
    const int rowNum = buildingNum / numCols;
    const int firstColOffset = -1 * int((btnHalfSize * (numCols - 1)) + (btnHalfPadding * (numCols - 1)));

    UIButton* button = new UIButton(uiManager->getNextComponentID(), true);
    button->setOffset(Vector2D(firstColOffset + colNum * btnSizeAndPadding, 20 + rowNum * btnSizeAndPadding));
    button->setSize(Vector2D(btnSize, btnSize));
    if (!blueprint->iconPath.empty())
      button->setButtonTexture(context->getRenderContext()->getSharedTexture(blueprint->iconPath));
    button->setButtonHighlightColour(BTN_PRESSED_COL, BTN_UNPRESSED_COL);
    button->setVerticalAlignment(alignmentStart);
    button->setHorizontalAlignment(alignmentCentre);
    button->setHighlightWidth(BTN_BORDER_SIZE);
    button->setGroup(buildingButtonGroup);

    uint gameDefID = blueprint->id;
    button->setMouseClickCallback([this, gameDefID, button, context](uint x, uint y) -> bool
      {
      if (button->isToggledDown())
        startBuildingPlacingMode(context, gameDefID);
      else
        endBuildingPlacingMode(context);
      return true;
      });

    subPanel->addChild(UIComponentPtr(button));
    ++buildingNum;
    });
  }

void HUDHandler::setupDebugPanel(GameContext* context)
  {
  UIManager* uiManager = context->getUIManager();
  debugPanel.reset(new SMDebugPanel(uiManager->getNextComponentID()));
  debugPanel->setHorizontalAlignment(alignmentStart);
  debugPanel->setVerticalAlignment(alignmentEnd);
  debugPanel->setSize(Vector2D(260, 150));
  debugPanel->setColour(Vector3D(0.2));
  uiManager->addComponent(debugPanel);
#ifdef NDEBUG
  debugPanel->setVisible(false, true);
#endif
  }

void HUDHandler::endBuildingPlacingMode(GameContext* gameContext)
  {
  if (placementHandler)
    {
    gameContext->removeInputHandler(placementHandler);
    placementHandler.reset();
    }
  deselectUI();
  updateBuildSelectionDetails(gameContext, 0);
  }

void HUDHandler::startBuildingPlacingMode(GameContext* gameContext, uint buildingBlueprintID)
  {
  SMGameContext::cast(gameContext)->getSelectionManager()->deselectAll(gameContext);

  if (placementHandler)
    {
    gameContext->removeInputHandler(placementHandler);
    placementHandler.reset();
    }
  placementHandler.reset(new WorldItemPlacementHandler(gameContext->getInputManager()->getNextHandlerID(), buildingBlueprintID));
  placementHandler->setEndHandlerCallback([this, gameContext]() { endBuildingPlacingMode(gameContext); });
  gameContext->addInputHandler(placementHandler);
  updateBuildSelectionDetails(gameContext, buildingBlueprintID);
  }

void HUDHandler::cleanUp(GameContext* context)
  {
  endBuildingPlacingMode(context);
  }

void HUDHandler::updateBuildSelectionDetails(GameContext* gameContext, uint buildingBlueprintID)
  {
  if (buildingBlueprintID == 0)
    {
    selectedBuildPanel->setVisible(false, true);
    selectedBuildIcon->setTexture(nullptr);
    selectedBuildIcon->invalidate();
    selectedBuildText->setText("");
    selectedBuildText->invalidate();
    return;
    }

  const SMGameActorBlueprint* blueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->getGameActorBlueprint(buildingBlueprintID);
  const GameActorBuildingBlueprint* buildingBlueprint = dynamic_cast<const GameActorBuildingBlueprint*>(blueprint);
  if (blueprint)
    {
    selectedBuildPanel->setVisible(true, true);
    if (!blueprint->iconPath.empty())
      selectedBuildIcon->setTexture(gameContext->getRenderContext()->getSharedTexture(blueprint->iconPath));
    else
      selectedBuildIcon->setTexture(nullptr);
    selectedBuildIcon->invalidate();

    string text = blueprint->displayName;
    if (!blueprint->description.empty())
      text += "\n\n" + blueprint->description;

    if (buildingBlueprint)
      {
      const SMGameActorBlueprint* constrPackBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->getGameActorBlueprint(buildingBlueprint->constructionPackID);
      if (constrPackBlueprint)
        text += "\n\n" + constrPackBlueprint->displayName + ": " + std::to_string(buildingBlueprint->constructionPackAmount);
      }

    selectedBuildText->setText(text);
    selectedBuildText->invalidate();
    }
  }