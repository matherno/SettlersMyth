//
// Created by matt on 28/01/18.
//

#include "HUDHandler.h"
#include "SMGameContext.h"
#include "Resources.h"

/*
 *  TowerFocusPanel
 */

void TowerFocusPanel::initialise(GameContext* context)
  {
  UIPanel::initialise(context);

  UIManager* uiManager = context->getUIManager();
  icon.reset(new UIPanel(uiManager->getNextComponentID()));
  icon->setOffset(Vector2D(20, 10));
  icon->setSize(Vector2D(75));
  icon->setColour(Vector3D(0.2));
  addChild(icon);

  static const Vector2D healthBarOffset = Vector2D(120, 15);
  static const Vector2D statsBarSize = Vector2D(200, 23);
  static const uint statsBarsGap = 10;
  static const uint statsTextGap = 5;
  static const uint statsTextSize = 18;

  healthBar.reset(new UIProgressBar(uiManager->getNextComponentID()));
  healthBar->setOffset(healthBarOffset);
  healthBar->setSize(statsBarSize);
  healthBar->setColour(Vector3D(0.2));
  healthBar->setBarColour(Vector3D(0.6, 0.1, 0.1));
  addChild(healthBar);

  healthText.reset(new UIText(uiManager->getNextComponentID()));
  healthText->setOffset(healthBarOffset + Vector2D(statsBarSize.x + statsTextGap, 0));
  healthText->setSize(statsBarSize);
  healthText->setFontSize(statsTextSize);
  healthText->setFontColour(Vector3D(0));
  healthText->showBackground(false);
  healthText->setTextCentreAligned(true, false);
  addChild(healthText);

  energyBar.reset(new UIProgressBar(uiManager->getNextComponentID()));
  energyBar->setOffset(healthBarOffset + Vector2D(0, statsBarSize.y + statsBarsGap));
  energyBar->setSize(statsBarSize);
  energyBar->setColour(Vector3D(0.2));
  energyBar->setBarColour(Vector3D(0, 0.2, 0.8));
  addChild(energyBar);

  energyText.reset(new UIText(uiManager->getNextComponentID()));
  energyText->setOffset(healthBarOffset + Vector2D(statsBarSize.x + statsTextGap, statsBarSize.y + statsBarsGap));
  energyText->setSize(statsBarSize);
  energyText->setFontSize(statsTextSize);
  energyText->setFontColour(Vector3D(0));
  energyText->showBackground(false);
  energyText->setTextCentreAligned(true, false);
  addChild(energyText);

  nameText.reset(new UIText(uiManager->getNextComponentID()));
  nameText->setOffset(Vector2D(20, -5));
  nameText->setSize(Vector2D(250, 40));
  nameText->setVerticalAlignment(alignmentEnd);
  nameText->setFontSize(30);
  nameText->setFontColour(Vector3D(0));
  nameText->showBackground(false);
  addChild(nameText);
  }

string createProgressString(int amount, int max)
  {
  std::ostringstream stringStream;
  stringStream << amount << "/" << max;
  return stringStream.str();
  }

void TowerFocusPanel::updateTowerInfo(GameContext* context)
  {
  SMGameContext* toGameContext = SMGameContext::cast(context);

//  if (TowerPtr focusedTower = toGameContext->getFocusedTower())
//    {
//    setVisible(true, true);
//
//    bool newFocusTower = false;
//    if (!activeFocusTower || focusedTower->getID() != activeFocusTower->getID())
//      {
//      activeFocusTower = focusedTower;
//      const TowerType* type = TowerFactory::getTowerType(activeFocusTower->getTowerType());
//      icon->setTexture(context->getRenderContext()->getSharedTexture(type->iconFilePath));
//      icon->invalidate();
//      nameText->setText(type->name);
//      nameText->invalidate();
//      newFocusTower = true;
//      }
//
//    const int healthPoints = focusedTower->getHealthPoints();
//    const int maxHealthPoints = focusedTower->getMaxHealthPoints();
//    if (UIProgressBar::updateProgressBar(healthBar.get(), healthPoints, maxHealthPoints) || newFocusTower)
//      {
//      healthText->setText(createProgressString(healthPoints, maxHealthPoints));
//      healthText->invalidate();
//      }
//
//    const int storedEnergy = focusedTower->getStoredEnergy();
//    const int maxEnergy = focusedTower->getMaxEnergy();
//    if(UIProgressBar::updateProgressBar(energyBar.get(), storedEnergy, maxEnergy) || newFocusTower)
//      {
//      energyText->setText(createProgressString(storedEnergy, maxEnergy));
//      energyText->invalidate();
//      }
//    }
//  else
//    {
//    setVisible(false, true);
//    activeFocusTower.reset();
//    }
  }


/*
 *  TODebugPanel
 */

void TODebugPanel::initialise(GameContext* context)
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

void TODebugPanel::updateDebugInfo(GameContext* context)
  {
  if (textComponent && isVisible() && refreshTimer.incrementTimer(context->getDeltaTime()))
    {
    SMGameContext* toGameContext = SMGameContext::cast(context);
    string text;
    text += "Game Time: " + mathernogl::formatTime(context->getGameTime()) + "\n";
    text += "Delta Time: " + std::to_string(toGameContext->getDeltaTime()) + " ms\n";
    text += "FPS: " + std::to_string(int(1.0f / ((float)toGameContext->getDeltaTime() / 1000.0f))) + "\n";

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
  mainUIPanel->setSize(Vector2D(300, 170));
  mainUIPanel->setColour(Vector3D(0.4, 0.3, 0.1));
  mainUIPanel->setHorizontalAlignment(alignmentEnd);
  mainUIPanel->setVerticalAlignment(alignmentEnd);
  mainUIPanel->setWidthMatchParent(true);
  uiManager->addComponent(mainUIPanel);

  setupTowerBuildPanel(context);
  setupTowerFocusPanel(context);
  setupDebugPanel(context);
  }

void HUDHandler::updateUI(GameContext* context)
  {
  towerFocusPanel->updateTowerInfo(context);
  debugPanel->updateDebugInfo(context);
  }

void HUDHandler::deselectTowerType()
  {
  towerButtonGroup->forceDeselectAll();
  }

void HUDHandler::toggleDebugPanel()
  {
  if (!debugPanel)
    return;
  debugPanel->setVisible(!debugPanel->isVisible(), true);
  }

void HUDHandler::setupTowerFocusPanel(GameContext* context)
  {
  UIManager* uiManager = context->getUIManager();
  towerFocusPanel.reset(new TowerFocusPanel(uiManager->getNextComponentID()));
  towerFocusPanel->setOffset(Vector2D(-10, 0));
  towerFocusPanel->setSize(Vector2D(600, 100));
  towerFocusPanel->setColour(Vector3D(0.3, 0.3, 0.25));
  towerFocusPanel->setVerticalAlignment(alignmentCentre);
  towerFocusPanel->setHorizontalAlignment(alignmentEnd);
  towerFocusPanel->setHeightMatchParent(true);
  towerFocusPanel->setPadding(10, 10);
  mainUIPanel->addChild(towerFocusPanel);
  }

void HUDHandler::setupTowerBuildPanel(GameContext* context)
  {
  UIManager* uiManager = context->getUIManager();
  SMGameContext* smGameContext = SMGameContext::cast(context);

  UIPanel* subPanel = new UIPanel(uiManager->getNextComponentID());
  subPanel->setOffset(Vector2D(0, 0));
  subPanel->setSize(Vector2D(100, 100));
  subPanel->setColour(Vector3D(0.35, 0.35, 0.3));
  subPanel->setVerticalAlignment(alignmentCentre);
  subPanel->setHorizontalAlignment(alignmentCentre);
  subPanel->setHeightMatchParent(true);
  subPanel->setWidthMatchParent(true);
  subPanel->setPadding(5, 5);
  mainUIPanel->addChild(UIComponentPtr(subPanel));

  towerButtonGroup.reset(new UIToggleButtonGroup());

  int buildingNum = 0;
  std::vector<IGameObjectDefPtr> buildings;
  smGameContext->getGameObjectFactory()->getGameObjectDefs(GameObjectType::building, &buildings);
  for (auto buildingDef : buildings)
    {
    UIButton* button = new UIButton(uiManager->getNextComponentID(), true);
    button->setOffset(Vector2D(50 + buildingNum * 80, 20));
    button->setSize(Vector2D(60, 60));
    if (!buildingDef->getIconFilePath().empty())
      button->setButtonTexture(context->getRenderContext()->getSharedTexture(buildingDef->getIconFilePath()));
    button->setButtonHighlightColour(Vector3D(0.3, 0.3, 1));
    button->setVerticalAlignment(alignmentStart);
    button->setHorizontalAlignment(alignmentStart);
    button->setHighlightWidth(3);
    button->setGroup(towerButtonGroup);
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
  debugPanel.reset(new TODebugPanel(uiManager->getNextComponentID()));
  debugPanel->setHorizontalAlignment(alignmentEnd);
  debugPanel->setSize(Vector2D(260, 260));
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
  deselectTowerType();
  }

void HUDHandler::startBuildingPlacingMode(GameContext* gameContext, uint buildingDefID)
  {
//  SMGameContext::cast(gameContext)->getSelectionManager()->deselectAll(gameContext);

  if (placementHandler)
    {
    gameContext->removeInputHandler(placementHandler);
    placementHandler.reset();
    }
  placementHandler.reset(new BuildingPlacementHandler(gameContext->getInputManager()->getNextHandlerID(), buildingDefID));
  placementHandler->setEndHandlerCallback([this, gameContext]() { endBuildingPlacingMode(gameContext); });
  gameContext->addInputHandler(placementHandler);
  }

void HUDHandler::cleanUp(GameContext* context)
  {
  endBuildingPlacingMode(context);
  }

