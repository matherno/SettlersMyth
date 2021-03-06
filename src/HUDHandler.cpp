//
// Created by matt on 28/01/18.
//

#include "HUDHandler.h"
#include "SMGameContext.h"
#include "Resources.h"
#include "Utils.h"
#include "GameActorTypes/GameActorBuilding.h"

#define FOCUS_PANEL_WIDTH      250
#define FOCUS_PANEL_MIN_HEIGHT 100

/*
 *  ActorFocusPanel
 */

void ActorFocusPanel::initialise(GameContext* context)
  {
  setColour(HUD_COL_BORDER);
  setSize(Vector2D(FOCUS_PANEL_WIDTH, FOCUS_PANEL_MIN_HEIGHT));
  UIPanel::initialise(context);
  }

void ActorFocusPanel::updateActorInfo(GameContext* context)
  {
  SMGameContext* smGameContext = SMGameContext::cast(context);

  if (SMGameActorPtr actor = smGameContext->getSelectionManager()->getFirstSelectedActor())
    {
    setVisible(true, true);
    if (!focusActor || focusActor->getID() != actor->getID())
      {
      if (focusActor)
        focusActor->removeSelectionHUD(context, this);
      focusActor = actor;
      const int height = focusActor->setupSelectionHUD(context, this);
      setSize(Vector2D(FOCUS_PANEL_WIDTH, std::max(height + 10, FOCUS_PANEL_MIN_HEIGHT)));
      invalidate();
      }
    else
      focusActor->updateSelectionHUD(context);
    }
  else
    {
    setVisible(false, true);
    if (focusActor)
      focusActor->removeSelectionHUD(context, this);
    focusActor.reset();
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
  }

void HUDHandler::updateUI(GameContext* context)
  {
  focusPanel->updateActorInfo(context);
  }

void HUDHandler::deselectUI()
  {
  buildingButtonGroup->forceDeselectAll();
  }

void HUDHandler::setupFocusPanel(GameContext* context)
  {
  UIManager* uiManager = context->getUIManager();
  focusPanel.reset(new ActorFocusPanel(uiManager->getNextComponentID()));
  focusPanel->setOffset(Vector2D(0, 20));
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