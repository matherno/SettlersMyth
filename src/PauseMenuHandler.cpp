//
// Created by matt on 3/04/18.
//

#include <UISystem/UIButton.h>
#include <UISystem/UIMessageBox.h>
#include "PauseMenuHandler.h"
#include "SaveLoadDlg.h"
#include "SMGameContext.h"
#include "SettingsDlg.h"
#include "Utils.h"

/*
 * Pause Menu parent UI component
 */

typedef std::function<void()> CallbackFunc;
class PauseMenuUI : public UIPanel
  {
private:
  CallbackFunc callback;
public:
  PauseMenuUI(uint id, CallbackFunc escPressedCallback) : UIPanel(id), callback(escPressedCallback) {}
  virtual void onEscapePressed(GameContext* context) override { callback(); }
  };


/*
 * Pause Menu Handler
 */

PauseMenuHandler::PauseMenuHandler(uint id) : GameActor(id)
  {}

typedef std::pair<string, OnMouseClickCallback> MenuOption;

void PauseMenuHandler::onAttached(GameContext* gameContext)
  {

  }

void PauseMenuHandler::onUpdate(GameContext* gameContext)
  {

  }

void PauseMenuHandler::onDetached(GameContext* gameContext)
  {
  }

void PauseMenuHandler::displayMenu(GameContext* gameContext)
  {
  UIManager* uiManager = gameContext->getUIManager();
  menuUI.reset(new PauseMenuUI(uiManager->getNextComponentID(), [this, gameContext](){ onReturnPressed(gameContext); }));
  menuUI->setWidthMatchParent(true);
  menuUI->setHeightMatchParent(true);
  menuUI->setVisible(false, false);
  gameContext->getUIManager()->addComponent(menuUI);
  gameContext->getUIManager()->pushModalComponent(menuUI);

  std::vector<MenuOption> menuButtons;

  //  RETURN
  menuButtons.emplace_back("Return", [this, gameContext](uint mouseX, uint mouseY)
    {
    onReturnPressed(gameContext);
    return true;
    });

  //  SAVE
  menuButtons.emplace_back("Save", [this, gameContext](uint mouseX, uint mouseY)
    {
    onSavePressed(gameContext);
    return true;
    });

  //  SETTINGS
  menuButtons.emplace_back("Settings", [this, gameContext](uint mouseX, uint mouseY)
    {
    onSettingsPressed(gameContext);
    return true;
    });

  //  QUIT
  menuButtons.emplace_back("Quit", [this, gameContext](uint mouseX, uint mouseY)
    {
    onQuitPressed(gameContext);
    return true;
    });

  //  load all the buttons
  const Vector2D buttonSize(150, 50);
  const float buttonPadding = 30;
  float buttonYOffset = (buttonPadding + buttonSize.y) * (menuButtons.size() - 1) * -0.5f;
  for (MenuOption& menuOption : menuButtons)
    {
    UIButton* button = new UIButton(uiManager->getNextComponentID(), false);
    button->setButtonText(menuOption.first, MENU_BTN_TEXT_COL, MENU_BTN_PRESS_COL, 40);
    button->setButtonHighlightColour(MENU_BTN_PRESS_COL, MENU_BTN_UNPRESS_COL);
    button->setButtonColour(MENU_BTN_BG_COL);
    button->setSize(buttonSize);
    button->setHorizontalAlignment(Alignment::alignmentCentre);
    button->setVerticalAlignment(Alignment::alignmentCentre);
    button->setOffset(Vector2D(0, buttonYOffset));
    button->setMouseClickCallback(menuOption.second);
    button->setHighlightWidth(MENU_BTN_BORDER_SIZE);
    menuUI->addChild(UIComponentPtr(button));
    buttonYOffset += buttonPadding + buttonSize.y;
    }
  }

void PauseMenuHandler::onReturnPressed(GameContext* gameContext)
  {
  gameContext->getUIManager()->removeComponent(menuUI->getID());
  gameContext->getUIManager()->popModalComponent();
  }

void PauseMenuHandler::onSavePressed(GameContext* gameContext)
  {
  menuUI->setVisible(false, true);
  UIManager* uiManager = gameContext->getUIManager();
  std::shared_ptr<SaveLoadDlg> saveLoadDlg(new SaveLoadDlg(uiManager->getNextComponentID(), SaveLoadDlg::modeSave));
  uiManager->addComponent(saveLoadDlg);
  uiManager->pushModalComponent(saveLoadDlg);

  //  On saving to a file
  uint id = saveLoadDlg->getID();
  saveLoadDlg->setSaveGameStateCallback([this, id, gameContext](string filePath)
        {
        SMGameContext::cast(gameContext)->saveGame(filePath);
        gameContext->getUIManager()->removeComponent(id);
        gameContext->getUIManager()->popModalComponent();
        onReturnPressed(gameContext);
        });

  //  On cancelled saving
  saveLoadDlg->setCancelledCallback([this, id, gameContext]()
        {
        gameContext->getUIManager()->removeComponent(id);
        gameContext->getUIManager()->popModalComponent();
        onCancelledSave(gameContext);
        });
  }

void PauseMenuHandler::onSettingsPressed(GameContext* gameContext)
  {
  UIManager* uiManager = gameContext->getUIManager();
  std::shared_ptr<SettingsDlg> settingsDlg(new SettingsDlg(uiManager->getNextComponentID()));
  uiManager->addComponent(settingsDlg);
  uiManager->pushModalComponent(settingsDlg);
  uint id = settingsDlg->getID();
  settingsDlg->setOnFinishedCallback([this, uiManager, id, gameContext]()
      {
      SMGameContext::cast(gameContext)->invalidateShadowMap();     
      uiManager->removeComponent(id);
      uiManager->popModalComponent();
      });
  }

void PauseMenuHandler::onQuitPressed(GameContext* gameContext)
  {
  UIMessageBox::popupMessageBox(gameContext->getUIManager(), "Are you sure you want to quit?", UIMessageBox::modeContinueCancel,
      [this, gameContext](UIMessageBox::MsgBoxResult result)
        {
        if (result == UIMessageBox::resultContinue)
          {
          gameContext->getUIManager()->popModalComponent();
          gameContext->getUIManager()->removeComponent(menuUI->getID());
          gameContext->endContext();
          }
        });
  }

void PauseMenuHandler::onCancelledSave(GameContext* gameContext)
  {
  menuUI->setVisible(true, true);
  menuUI->setVisible(false, false);
  }
