
#include <UISystem/UIPanel.h>
#include <UISystem/UIButton.h>
#include <UISystem/UIList.h>
#include "SMMainMenuContext.h"
#include "Resources.h"
#include "SettingsDlg.h"


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
    endContext();
    return true;
    });

  //  LOAD
  menuButtons.emplace_back("Load", [this](uint mouseX, uint mouseY)
    {
    currentOutcome.optionSelected = optionLoad;
    showLoadDlg();
    return true;
    });

  //  SETTINGS
  menuButtons.emplace_back("Settings", [this](uint mouseX, uint mouseY)
    {
    currentOutcome.optionSelected = optionSettings;
    showSettingsDlg();
    return true;
    });

  //  QUIT
  menuButtons.emplace_back("Quit", [this](uint mouseX, uint mouseY)
    {
    currentOutcome.optionSelected = optionQuit;
    endContext();
    return true;
    });

  UIPanel* background = new UIPanel(getUIManager()->getNextComponentID());
  background->setColour(Vector3D(0.25, 0.125, 0.1));
  background->setWidthMatchParent(true);
  background->setHeightMatchParent(true);
  getUIManager()->addComponent(UIComponentPtr(background));

  UIPanel* buttonBG = new UIPanel(getUIManager()->getNextComponentID());
  buttonBG->setSize(Vector2D(300, 0));
  buttonBG->setColour(Vector3D(0.09, 0.07, 0.04));
  buttonBG->setPadding(0, 50);
  buttonBG->setHeightMatchParent(true);
  buttonBG->setHorizontalAlignment(alignmentCentre);
  background->addChild(UIComponentPtr(buttonBG));

  //  load all the buttons
  const Vector2D buttonSize(250, 70);
  const Vector3D colour(0.3, 0.3, 0.3);
  const Vector3D pressColour(0.2, 0.3, 0.5);
  const float buttonPadding = 50;
  float buttonYOffset = 100;
  for (MenuOption& menuOption : menuButtons)
    {
    UIButton* button = new UIButton(getUIManager()->getNextComponentID(), false);
    button->setButtonText(menuOption.first, colour, pressColour, 60);
    button->setButtonHighlightColour(pressColour, colour);
    button->setSize(buttonSize);
    button->setHorizontalAlignment(Alignment::alignmentCentre);
    button->setOffset(Vector2D(0, buttonYOffset));
    button->setMouseClickCallback(menuOption.second);
    buttonBG->addChild(UIComponentPtr(button));
    buttonYOffset += buttonPadding + buttonSize.y;
    }
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
  getUIManager()->pushModalComponent(saveLoadDlg);

  //  On loaded from a file
  saveLoadDlg->setLoadGameStateCallback([this](std::shared_ptr<SMGameState> state)
      {
      currentOutcome.optionSelected = optionLoad;
      currentOutcome.loadedState = state;
      endContext();
      });

  //  On cancelled loading
  uint id = saveLoadDlg->getID();
  saveLoadDlg->setCancelledCallback([this, id]()
      {
      getUIManager()->removeComponent(id);
      getUIManager()->popModalComponent();
      });
  }

void SMMainMenuContext::showSettingsDlg()
  {
  std::shared_ptr<SettingsDlg> settingsDlg(new SettingsDlg(getUIManager()->getNextComponentID()));
  getUIManager()->addComponent(settingsDlg);
  getUIManager()->pushModalComponent(settingsDlg);
  uint id = settingsDlg->getID();
  settingsDlg->setOnFinishedCallback([this, id]()
     {
     getUIManager()->removeComponent(id);
     getUIManager()->popModalComponent();
     });
  }