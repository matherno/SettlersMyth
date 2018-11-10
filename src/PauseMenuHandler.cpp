//
// Created by matt on 3/04/18.
//

#include <UISystem/UIButton.h>
#include <UISystem/UIMessageBox.h>
#include "PauseMenuHandler.h"
#include "SaveLoadDlg.h"
#include "SMGameContext.h"
#include "SettingsDlg.h"

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

  const Vector3D colour(0.3, 0.3, 0.3);
  UIPanel* buttonBGBorder = new UIPanel(uiManager->getNextComponentID());
  buttonBGBorder->setSize(Vector2D(200, 500));
  buttonBGBorder->setColour(colour);
  buttonBGBorder->setHorizontalAlignment(alignmentCentre);
  buttonBGBorder->setVerticalAlignment(alignmentCentre);
  menuUI->addChild(UIComponentPtr(buttonBGBorder));

  UIPanel* buttonBG = new UIPanel(uiManager->getNextComponentID());
  buttonBG->setColour(Vector3D(0.12, 0.07, 0.04));
  buttonBG->setWidthMatchParent(true);
  buttonBG->setHeightMatchParent(true);
  buttonBG->setPadding(4, 4);
  buttonBGBorder->addChild(UIComponentPtr(buttonBG));

  //  load all the buttons
  const Vector2D buttonSize(150, 50);
  const Vector3D pressColour(0.2, 0.3, 0.5);
  const float buttonPadding = 30;
  float buttonYOffset = 80;
  for (MenuOption& menuOption : menuButtons)
    {
    UIButton* button = new UIButton(uiManager->getNextComponentID(), false);
    button->setButtonText(menuOption.first, colour, pressColour, 40);
    button->setButtonHighlightColour(pressColour, colour);
    button->setSize(buttonSize);
    button->setHorizontalAlignment(Alignment::alignmentCentre);
    button->setOffset(Vector2D(0, buttonYOffset));
    button->setMouseClickCallback(menuOption.second);
    button->setHighlightWidth(3);
    buttonBG->addChild(UIComponentPtr(button));
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
  saveLoadDlg->setSaveGameStateCallback([this, id, gameContext]()
        {
        std::shared_ptr<SMGameState> state(new SMGameState());
        SMGameContext::cast(gameContext)->getGameState(state.get());
        gameContext->getUIManager()->removeComponent(id);
        gameContext->getUIManager()->popModalComponent();
        onReturnPressed(gameContext);
        return state;
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
  settingsDlg->setOnFinishedCallback([this, uiManager, id]()
     {
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
