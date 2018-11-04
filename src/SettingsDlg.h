#pragma once

#include <UISystem/UIPanel.h>
#include <UISystem/UIList.h>
#include <UISystem/UIEditText.h>
#include <GameSystem/GameConfig.h>

/*
*   
*/

typedef std::function<void()> OnFinishedCallback;

class SettingsDlg : public UIPanel
  {
private:
  std::shared_ptr<GameConfigValueMap> configValueMap;
  OnFinishedCallback onFinishedFunc;

  // each of these functions are called when configValueMap needs updating with the current control values
  std::vector<std::function<void(GameConfigValueMap* valueMap)>> updateValueMapFuncs;

public:
  SettingsDlg(uint id);

  virtual void initialise(GameContext* context) override;
  virtual void onEscapePressed(GameContext* context) override;
  void setOnFinishedCallback(OnFinishedCallback func) { onFinishedFunc = func; }

protected:
  void onCancelPressed();
  void onApplyPressed(GameContext* context);

  void addOptions(GameContext* context, UIPanel* parent);
  void addEditOption(GameContext* context, UIPanel* parent, uint yOffset, const GameConfigManager::ConfigOption* option);
  void addBoolOption(GameContext* context, UIPanel* parent, uint yOffset, const GameConfigManager::ConfigOption* option);
  };
