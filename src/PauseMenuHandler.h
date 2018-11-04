#pragma once

#include <memory>
#include <UISystem/UIPanel.h>

/*
*   
*/

class PauseMenuHandler : public GameActor
  {
private:
  std::shared_ptr<UIPanel> menuUI;

public:
  PauseMenuHandler(uint id);
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;

  void displayMenu(GameContext* context);

protected:
  void onReturnPressed(GameContext* gameContext);
  void onSavePressed(GameContext* gameContext);
  void onSettingsPressed(GameContext* gameContext);
  void onQuitPressed(GameContext* gameContext);
  void onCancelledSave(GameContext* gameContext);
  };
