//
// Created by matt on 28/01/18.
//

#include "HUDHandler.h"
#include "SMGameContext.h"
#include "Resources.h"
#include "Utils.h"


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

  resourceText.reset(new UIText(uiManager->getNextComponentID()));
  resourceText->setOffset(Vector2D(0, 90));
  resourceText->setSize(Vector2D(0, 100));
  resourceText->setWidthMatchParent(true);
  resourceText->setPadding(10, 10);
  resourceText->setFontSize(22);
  resourceText->setFontColour(Vector3D(0));
  resourceText->showBackground(false);
  subPanel->addChild(resourceText);
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
      const IGameObjectDef* actorDef = actor->getDef();
      if (!actorDef->getIconFilePath().empty())
        icon->setTexture(context->getRenderContext()->getSharedTexture(actorDef->getIconFilePath()));
      else
        icon->setTexture(nullptr);
      icon->invalidate();
      nameText->setText(actorDef->getDisplayName());
      nameText->invalidate();

      }

    bool first = true;
    string resText;
    actor->forEachResource([&](uint id, uint amount)
      {
      IGameObjectDefPtr resDef = smGameContext->getGameObjectFactory()->getGameObjectDef(id);
      if (resDef)
        {
        if (!first)
          resText += "\n";
        resText += resDef->getDisplayName() + ":" + std::to_string(amount);
        first = false;
        }
      }, true, true);

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
    SMGameContext* toGameContext = SMGameContext::cast(context);
    string text;
    text += "Game Time: " + mathernogl::formatTime(context->getGameTime()) + "\n";
    text += "Delta Time: " + std::to_string(toGameContext->getDeltaTime()) + " ms\n";
    text += "FPS: " + std::to_string(int(1.0f / ((float)toGameContext->getDeltaTime() / 1000.0f))) + "\n";
    text += "Speed: " + std::to_string(toGameContext->getSpeed()) + "\n";

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
  mainUIPanel->setSize(Vector2D(150, 170));
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
  focusPanel->setSize(Vector2D(200, 200));
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

  buildingButtonGroup.reset(new UIToggleButtonGroup());

  static const int numCols = 2;
  static const int btnSize = 45;
  static const int btnHalfSize = int(btnSize * 0.5);
  static const int btnPadding = 13;
  static const int btnHalfPadding = int(btnPadding * 0.5);
  static const int btnSizeAndPadding = btnSize + btnPadding;

  int buildingNum = 0;
  std::vector<IGameObjectDefPtr> buildings;
  smGameContext->getGameObjectFactory()->getGameObjectDefs(GameObjectType::building, &buildings);
  for (auto buildingDef : buildings)
    {
    const int colNum = buildingNum % numCols;
    const int rowNum = buildingNum / numCols;
    const int firstColOffset = -1 * int((btnHalfSize * (numCols - 1)) + (btnHalfPadding * (numCols - 1)));

    UIButton* button = new UIButton(uiManager->getNextComponentID(), true);
    button->setOffset(Vector2D(firstColOffset + colNum * btnSizeAndPadding, 20 + rowNum * btnSizeAndPadding));
    button->setSize(Vector2D(btnSize, btnSize));
    if (!buildingDef->getIconFilePath().empty())
      button->setButtonTexture(context->getRenderContext()->getSharedTexture(buildingDef->getIconFilePath()));
    button->setButtonHighlightColour(BTN_PRESSED_COL, BTN_UNPRESSED_COL);
    button->setVerticalAlignment(alignmentStart);
    button->setHorizontalAlignment(alignmentCentre);
    button->setHighlightWidth(BTN_BORDER_SIZE);
    button->setGroup(buildingButtonGroup);

    uint gameDefID = buildingDef->getID();
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
    }
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
//#ifdef NDEBUG
  debugPanel->setVisible(false, true);
//#endif
  }

void HUDHandler::endBuildingPlacingMode(GameContext* gameContext)
  {
  if (placementHandler)
    {
    gameContext->removeInputHandler(placementHandler);
    placementHandler.reset();
    }
  deselectUI();
  }

void HUDHandler::startBuildingPlacingMode(GameContext* gameContext, uint buildingDefID)
  {
  SMGameContext::cast(gameContext)->getSelectionManager()->deselectAll(gameContext);

  if (placementHandler)
    {
    gameContext->removeInputHandler(placementHandler);
    placementHandler.reset();
    }
  placementHandler.reset(new WorldItemPlacementHandler(gameContext->getInputManager()->getNextHandlerID(), buildingDefID));
  placementHandler->setEndHandlerCallback([this, gameContext]() { endBuildingPlacingMode(gameContext); });
  gameContext->addInputHandler(placementHandler);
  }

void HUDHandler::cleanUp(GameContext* context)
  {
  endBuildingPlacingMode(context);
  }

