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
  SMGameActorPtr focusActor;

public:
  ActorFocusPanel(uint id) : UIPanel(id) {}
  void updateActorInfo(GameContext* context);

private:
  virtual void initialise(GameContext* context) override;
  };
  

class HUDHandler
  {
private:
  std::shared_ptr<UIPanel> mainUIPanel;
  std::shared_ptr<ActorFocusPanel> focusPanel;
  UIToggleButtonGroupPtr buildingButtonGroup;
  std::shared_ptr<WorldItemPlacementHandler> placementHandler;
  
  std::shared_ptr<UIPanel> selectedBuildPanel;
  std::shared_ptr<UIPanel> selectedBuildIcon;
  std::shared_ptr<UIText> selectedBuildText;

public:
  void initialiseUI(GameContext* context);
  void updateUI(GameContext* context);
  void cleanUp(GameContext* context);
  void deselectUI();

protected:
  void setupFocusPanel(GameContext* context);
  void setupBuildPanel(GameContext* context);
  void startBuildingPlacingMode(GameContext* gameContext, uint buildingBlueprintID);
  void endBuildingPlacingMode(GameContext* gameContext);
  void updateBuildSelectionDetails(GameContext* gameContext, uint buildingBlueprintID);
  };
