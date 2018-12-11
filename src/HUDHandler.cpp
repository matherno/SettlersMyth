//
// Created by matt on 28/01/18.
//

#include "HUDHandler.h"
#include "SMGameContext.h"
#include "Resources.h"

/*
 *  ActorFocusPanel
 */

void ActorFocusPanel::initialise(GameContext* context)
  {
  UIPanel::initialise(context);

  UIManager* uiManager = context->getUIManager();
  icon.reset(new UIPanel(uiManager->getNextComponentID()));
  icon->setOffset(Vector2D(20, 10));
  icon->setSize(Vector2D(75));
  icon->setColour(Vector3D(0.2));
  addChild(icon);

  nameText.reset(new UIText(uiManager->getNextComponentID()));
  nameText->setOffset(Vector2D(20, -10));
  nameText->setSize(Vector2D(250, 40));
  nameText->setVerticalAlignment(alignmentEnd);
  nameText->setFontSize(30);
  nameText->setFontColour(Vector3D(0));
  nameText->showBackground(false);
  addChild(nameText);

  resourceText.reset(new UIText(uiManager->getNextComponentID()));
  resourceText->setOffset(Vector2D(-10, 0));
  resourceText->setSize(Vector2D(400, 0));
  resourceText->setHorizontalAlignment(alignmentEnd);
  resourceText->setVerticalAlignment(alignmentCentre);
  resourceText->setHeightMatchParent(true);
  resourceText->setPadding(0, 10);
  resourceText->setFontSize(30);
  resourceText->setFontColour(Vector3D(0));
  resourceText->showBackground(false);
  addChild(resourceText);
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
    for (const auto& pair : *actor->getStoredResources())
      {
      IGameObjectDefPtr resDef = smGameContext->getGameObjectFactory()->getGameObjectDef(pair.first);
      if (resDef)
        {
        if (!first)
          resText += ", ";
        resText += resDef->getDisplayName() + ":" + std::to_string(pair.second);
        first = false;
        }
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
  focusPanel->setOffset(Vector2D(-10, 0));
  focusPanel->setSize(Vector2D(600, 100));
  focusPanel->setColour(Vector3D(0.3, 0.3, 0.25));
  focusPanel->setVerticalAlignment(alignmentCentre);
  focusPanel->setHorizontalAlignment(alignmentEnd);
  focusPanel->setHeightMatchParent(true);
  focusPanel->setPadding(10, 10);
  mainUIPanel->addChild(focusPanel);
  }

void HUDHandler::setupBuildPanel(GameContext* context)
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

  buildingButtonGroup.reset(new UIToggleButtonGroup());

  int buildingNum = 0;
  std::vector<IGameObjectDefPtr> buildings;
  smGameContext->getGameObjectFactory()->getGameObjectDefs(GameObjectType::staticObject, &buildings);
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

