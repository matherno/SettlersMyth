#pragma once

#include <GameSystem/GameContextImpl.h>
#include "SaveLoadDlg.h"
#include "SMSettings.h"

/*
*   
*/

class TOMainMenuContext : public GameContextImpl
  {
public:
  enum Option
    {
    optionNone,
    optionNew,
    optionQuit,
    optionLoad,
    optionSettings,
    };

  struct MainMenuOutcome
    {
    Option optionSelected = optionNone;
    std::shared_ptr<TOGameState> loadedState;
    bool isNewGame() const { return optionSelected == optionNew; }
    bool isLoadGame() const { return optionSelected == optionLoad; }
    bool isQuitGame() const { return optionSelected == optionQuit; }
    };

private:
  MainMenuOutcome currentOutcome;
  std::shared_ptr<SMSettings> settings;

public:
  TOMainMenuContext(const RenderContextPtr& renderContext, std::shared_ptr<SMSettings> settings);
  MainMenuOutcome getSelectedOption() { return currentOutcome; }
  virtual FontPtr getDefaultFont() override;
  virtual bool initialise() override;
  SMSettings* getSettings() { return settings.get(); }

  static MainMenuOutcome doMainMenu(RenderContextPtr renderContext, std::shared_ptr<SMSettings> settings);
  inline static TOMainMenuContext* cast(GameContext* context)
    {
    TOMainMenuContext* toContext = dynamic_cast<TOMainMenuContext*>(context);
    ASSERT(toContext, "Given game context wasn't a TOMainMenuContext!");
    return toContext;
    }

protected:
  void showLoadDlg();
  void showSettingsDlg();
  };
