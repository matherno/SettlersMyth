
#include <UISystem/UIPanel.h>
#include <UISystem/UIButton.h>
#include <UISystem/UIList.h>
#include "SMMainMenuContext.h"
#include "Resources.h"
#include "SettingsDlg.h"
#include "Utils.h"


SMMainMenuContext::SMMainMenuContext(const RenderContextPtr& renderContext, std::shared_ptr<SMSettings> settings)
  : GameContextImpl(renderContext), settings(settings)
  {}

typedef std::pair<string, OnMouseClickCallback> MenuOption;

bool SMMainMenuContext::initialise()
  {
  bool result = GameContextImpl::initialise();

  std::vector<MenuOption> menuButtons;

  //  NEW
  menuButtons.emplace_back("New", [this](uint mouseX, uint mouseY)
    {
    currentOutcome.optionSelected = optionNew;
    closeCurrentDlg();
    endContext();
    return true;
    });

  //  LOAD
  menuButtons.emplace_back("Load", [this](uint mouseX, uint mouseY)
    {
    currentOutcome.optionSelected = optionLoad;
    closeCurrentDlg();
    showLoadDlg();
    return true;
    });

  //  SETTINGS
  menuButtons.emplace_back("Settings", [this](uint mouseX, uint mouseY)
    {
    currentOutcome.optionSelected = optionSettings;
    closeCurrentDlg();
    showSettingsDlg();
    return true;
    });

  //  QUIT
  menuButtons.emplace_back("Quit", [this](uint mouseX, uint mouseY)
    {
    currentOutcome.optionSelected = optionQuit;
    closeCurrentDlg();
    endContext();
    return true;
    });

  TexturePtr backgroundImage = getRenderContext()->getSharedTexture(IMAGE_MENU_BACKGROUND);
  UIPanel* background = new UIPanel(getUIManager()->getNextComponentID());
  background->setColour(Vector3D(0.25, 0.125, 0.1));
  background->setTexture(backgroundImage);
  background->setSize(Vector2D(backgroundImage->width, backgroundImage->height));
  background->setHorizontalAlignment(Alignment::alignmentCentre);
  background->setVerticalAlignment(Alignment::alignmentCentre);
  background->setCanHitWithMouse(false);
  getUIManager()->addComponent(UIComponentPtr(background));

  UIToggleButtonGroupPtr btnGroup(new UIToggleButtonGroup());

  //  load all the buttons
  const Vector2D buttonSize(200, 50);
  const float buttonPadding = 40;
  float buttonYOffset = (buttonPadding + buttonSize.y) * menuButtons.size() * -0.5f;
  for (MenuOption& menuOption : menuButtons)
    {
    UIButton* button = new UIButton(getUIManager()->getNextComponentID(), true);
    button->setButtonText(menuOption.first, MENU_BTN_TEXT_COL, MENU_BTN_PRESS_COL, 50);
    button->setButtonHighlightColour(MENU_BTN_PRESS_COL, MENU_BTN_UNPRESS_COL);
    button->setButtonColour(MENU_BTN_BG_COL);
    button->setHighlightWidth(MENU_BTN_BORDER_SIZE);
    button->setVerticalAlignment(Alignment::alignmentCentre);
    button->setSize(buttonSize);
    button->setOffset(Vector2D(200, buttonYOffset));
    button->setMouseClickCallback(menuOption.second);
    button->setGroup(btnGroup);
    getUIManager()->addComponent(UIComponentPtr(button));
    buttonYOffset += buttonPadding + buttonSize.y;
    }

  UIText* gameTitleText = new UIText(getUIManager()->getNextComponentID());
  gameTitleText->setText(GAME_TITLE);
  gameTitleText->setFontSize(100);
  gameTitleText->setHorizontalAlignment(Alignment::alignmentCentre);
  gameTitleText->setVerticalAlignment(Alignment::alignmentStart);
  gameTitleText->setOffset(Vector2D(0, 100));
  gameTitleText->setSize(Vector2D(700, 100));
  gameTitleText->setTextCentreAligned(true, true);
  gameTitleText->showBackground(false);
  getUIManager()->addComponent(UIComponentPtr(gameTitleText));

  return result;
  }

FontPtr SMMainMenuContext::getDefaultFont()
  {
  return getRenderContext()->getSharedFont(FONT_DEFAULT_FNT, FONT_DEFAULT_GLYPHS, FONT_DEFAULT_SCALING);
  }

SMMainMenuContext::MainMenuOutcome SMMainMenuContext::doMainMenu(RenderContextPtr renderContext, std::shared_ptr<SMSettings> settings)
  {
  mathernogl::logInfo("Loading main menu...");
  SMMainMenuContext mainMenuContext(renderContext, settings);
  mainMenuContext.initialise();
  while(!mainMenuContext.isContextEnded() && renderContext->isWindowOpen())
    {
    mainMenuContext.startFrame();
    mainMenuContext.processInputStage();
    mainMenuContext.processUpdateStage();
    mainMenuContext.processDrawStage();
    mainMenuContext.endFrame(30);
    }
  SMMainMenuContext::MainMenuOutcome outcome = mainMenuContext.getSelectedOption();
  mainMenuContext.cleanUp();
  renderContext->reset();
  return outcome;
  }

void SMMainMenuContext::showLoadDlg()
  {
  std::shared_ptr<SaveLoadDlg> saveLoadDlg(new SaveLoadDlg(getUIManager()->getNextComponentID(), SaveLoadDlg::modeLoad));
  getUIManager()->addComponent(saveLoadDlg);
  currentDlg = saveLoadDlg;

  //  On loaded from a file
  saveLoadDlg->setLoadGameStateCallback([this](string saveFile)
      {
      currentOutcome.optionSelected = optionLoad;
      currentOutcome.saveFile = saveFile;
      closeCurrentDlg();
      endContext();
      });

  //  On cancelled loading
  saveLoadDlg->setCancelledCallback([this]()
      {
      closeCurrentDlg();
      });
  }

void SMMainMenuContext::showSettingsDlg()
  {
  std::shared_ptr<SettingsDlg> settingsDlg(new SettingsDlg(getUIManager()->getNextComponentID()));
  getUIManager()->addComponent(settingsDlg);
  settingsDlg->setOnFinishedCallback([this]()
     {
     closeCurrentDlg();
     });
  currentDlg = settingsDlg;
  }

void SMMainMenuContext::closeCurrentDlg()
  {
  if (!currentDlg)
    return;
  getUIManager()->removeComponent(currentDlg->getID());
  currentDlg.reset();
  }
