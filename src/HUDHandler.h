#pragma once

#include <UISystem/UIPanel.h>
#include <UISystem/UIProgressBar.h>
#include <UISystem/UIButton.h>
#include <UISystem/UIText.h>
#include "WorldItemPlacementHandler.h"
#include "SMGameActor.h"

/*
*   
*/

class ActorFocusPanel : public UIPanel
  {
private:
  std::shared_ptr<UIPanel> icon;
  std::shared_ptr<UIText> nameText;
  std::shared_ptr<UIText> resourceText;
  SMGameActorPtr focusActor;

public:
  ActorFocusPanel(uint id) : UIPanel(id) {}
  void updateActorInfo(GameContext* context);

private:
  virtual void initialise(GameContext* context) override;
  };

class SMDebugPanel : public UIPanel
  {
private:
  std::shared_ptr<UIText> textComponent;
  Timer refreshTimer;

public:
  SMDebugPanel(uint id) : UIPanel(id) {}
  void updateDebugInfo(GameContext* context);
private:
  virtual void initialise(GameContext* context) override;
  };

class HUDHandler
  {
private:
  std::shared_ptr<UIPanel> mainUIPanel;
  std::shared_ptr<ActorFocusPanel> focusPanel;
  std::shared_ptr<SMDebugPanel> debugPanel;
  UIToggleButtonGroupPtr buildingButtonGroup;
  std::shared_ptr<WorldItemPlacementHandler> placementHandler;

public:
  void initialiseUI(GameContext* context);
  void updateUI(GameContext* context);
  void cleanUp(GameContext* context);
  void deselectUI();
  void toggleDebugPanel();

protected:
  void setupFocusPanel(GameContext* context);
  void setupBuildPanel(GameContext* context);
  void setupDebugPanel(GameContext* context);
  void startBuildingPlacingMode(GameContext* gameContext, uint buildingDefID);
  void endBuildingPlacingMode(GameContext* gameContext);
  };
