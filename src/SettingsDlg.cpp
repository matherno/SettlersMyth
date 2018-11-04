//
// Created by matt on 21/04/18.
//

#include <UISystem/UICheckButton.h>
#include "SettingsDlg.h"
#include "SMMainMenuContext.h"
#include "SMGameContext.h"

SettingsDlg::SettingsDlg(uint id) : UIPanel(id)
  {}

void SettingsDlg::initialise(GameContext* context)
  {
  setColour(Vector3D(0.2, 0.15, 0.15));
  setVerticalAlignment(Alignment::alignmentCentre);
  setHorizontalAlignment(Alignment::alignmentCentre);
  setSize(Vector2D(800, 630));
  UIPanel::initialise(context);

  UIManager* uiManager = context->getUIManager();
  UIPanel* subPanel = new UIPanel(uiManager->getNextComponentID());
  subPanel->setPadding(5, 5);
  subPanel->setColour(Vector3D(0.4, 0.3, 0.3));
  subPanel->setWidthMatchParent(true);
  subPanel->setHeightMatchParent(true);
  addChild(UIComponentPtr(subPanel));

  UIPanel* optionsBorder = new UIList(uiManager->getNextComponentID());
  optionsBorder->setOffset(Vector2D(0, 40));
  optionsBorder->setSize(Vector2D(730, 505));
  optionsBorder->setHorizontalAlignment(Alignment::alignmentCentre);
  optionsBorder->setColour(Vector3D(0.35, 0.25, 0.15));
  subPanel->addChild(UIComponentPtr(optionsBorder));

  UIPanel* optionsBG = new UIPanel(uiManager->getNextComponentID());
  optionsBG->setVerticalAlignment(Alignment::alignmentCentre);
  optionsBG->setHorizontalAlignment(Alignment::alignmentCentre);
  optionsBG->setWidthMatchParent(true);
  optionsBG->setHeightMatchParent(true);
  optionsBG->setPadding(5, 5);
  optionsBG->setColour(Vector3D(0.4, 0.3, 0.2));
  optionsBorder->addChild(UIComponentPtr(optionsBG));

  addOptions(context, optionsBG);

  float buttonOffsetX = -40;
  auto createButton = [uiManager, subPanel, &buttonOffsetX](string text, OnMouseClickCallback mouseClickCallback)
    {
    static const Vector2D buttonSize(100, 30);
    UIButton* button = new UIButton(uiManager->getNextComponentID(), false);
    button->setSize(buttonSize);
    button->setOffset(Vector2D(buttonOffsetX, -25));
    button->setButtonColour(Vector3D(0.5, 0.3, 0.1));
    button->setButtonText(text, Vector3D(0), Vector3D(0.40, 0.20, 0.05), 25);
    button->setHighlightWidth(2);
    button->setVerticalAlignment(Alignment::alignmentEnd);
    button->setHorizontalAlignment(Alignment::alignmentEnd);
    button->setButtonHighlightColour(Vector3D(0.40, 0.20, 0.05), Vector3D(0.45, 0.25, 0.07));
    button->setMouseClickCallback(mouseClickCallback);
    subPanel->addChild(UIComponentPtr(button));
    buttonOffsetX -= 25 + buttonSize.x;
    };

  createButton("Cancel", [this](uint mouseX, uint mouseY){ onCancelPressed(); return true; });
  createButton("Apply", [this, context](uint mouseX, uint mouseY){ onApplyPressed(context); return true; });
  }

SMSettings* getSettingsHandler(GameContext* context)
  {
  if(TOMainMenuContext* toContext = dynamic_cast<TOMainMenuContext*>(context))
    return toContext->getSettings();
  if(TOGameContext* toContext = dynamic_cast<TOGameContext*>(context))
    return toContext->getSettings();
  return nullptr;
  }

void SettingsDlg::onCancelPressed()
  {
  if (onFinishedFunc)
    onFinishedFunc();
  }

void SettingsDlg::onApplyPressed(GameContext* context)
  {
  for (auto func : updateValueMapFuncs)
    func(configValueMap.get());
  getSettingsHandler(context)->setActiveValueMap(configValueMap);
  getSettingsHandler(context)->writeSettingsFile(configValueMap.get());
  if (onFinishedFunc)
    onFinishedFunc();
  }

void SettingsDlg::addOptions(GameContext* context, UIPanel* parent)
  {
  uint yOffset = 15;
  SMSettings* settingsHandler = getSettingsHandler(context);
  configValueMap.reset(new GameConfigValueMap(*settingsHandler->getActiveValueMap()));
  for (uint optionIdx = 0; optionIdx < settingsHandler->getConfigManager()->getNumOptions(); ++optionIdx)
    {
    const GameConfigManager::ConfigOption* option = settingsHandler->getConfigManager()->getOption(optionIdx);
    switch(option->type)
      {
      case GameConfigManager::typeInt:
        addEditOption(context, parent, yOffset, option);
        break;
      case GameConfigManager::typeBool:
        addBoolOption(context, parent, yOffset, option);
        break;
      case GameConfigManager::typeFloat:
        addEditOption(context, parent, yOffset, option);
        break;
      case GameConfigManager::typeString:
        addEditOption(context, parent, yOffset, option);
        break;
      default:
        ASSERT(false, "Missing control for option type: " + std::to_string(option->type));
        break;
      }
    yOffset += 35;
    }
  }

#define OPTION_HEIGHT 30

void addOptionLabel(GameContext* context, UIPanel* parent, uint yOffset, const GameConfigManager::ConfigOption* option)
  {
  UIText* optionName = new UIText(context->getUIManager()->getNextComponentID());
  optionName->setSize(Vector2D(300, 30));
  optionName->setOffset(Vector2D(20, yOffset));
  optionName->setText(option->namePretty);
  optionName->setTextCentreAligned(true, false);
  optionName->showBackground(false);
  optionName->setFontSize(27);
  parent->addChild(UIComponentPtr(optionName));
  }

void SettingsDlg::addEditOption(GameContext* context, UIPanel* parent, uint yOffset, const GameConfigManager::ConfigOption* option)
  {
  addOptionLabel(context, parent, yOffset, option);

  UIEditText::EditType editType = UIEditText::typeAll;
  string value = "";
  uint width = 300, charLimit = 10;
  switch(option->type)
    {
    case GameConfigManager::typeInt:
      {
      int intValue = 0;
      configValueMap->getIntValue(option->index, &intValue);
      value = std::to_string(intValue);
      editType = UIEditText::typeUInt;
      width = 100;
      charLimit = 7;
      break;
      }
    case GameConfigManager::typeFloat:
      {
      float floatValue = 0;
      configValueMap->getFloatValue(option->index, &floatValue);
      value = mathernogl::stringFormat("%.2f", floatValue);
      editType = UIEditText::typeFloat;
      width = 100;
      charLimit = 7;
      break;
      }
    case GameConfigManager::typeString:
      configValueMap->getStringValue(option->index, &value);
      editType = UIEditText::typeAll;
      width = 300;
      charLimit = 20;
      break;
    }

  std::shared_ptr<UIEditText> optionEditFld(new UIEditText(context->getUIManager()->getNextComponentID()));
  optionEditFld->setHorizontalAlignment(alignmentEnd);
  optionEditFld->setSize(Vector2D(width, 30));
  optionEditFld->setOffset(Vector2D(-20, yOffset));
  optionEditFld->setText(value);
  optionEditFld->setTextCentreAligned(true, false);
  optionEditFld->setColour(Vector3D(0.5));
  optionEditFld->setFontSize(23);
  optionEditFld->setCharacterLimit(charLimit);
  optionEditFld->setEditType(editType);
  parent->addChild(optionEditFld);

  updateValueMapFuncs.emplace_back([optionEditFld, option](GameConfigValueMap* valueMap)
    {
    if(option->type == GameConfigManager::typeString)
      valueMap->setStringValue(option->index, optionEditFld->getText());
    else if(option->type == GameConfigManager::typeFloat)
      valueMap->setFloatValue(option->index, optionEditFld->getTextFloatValue());
    else if(option->type == GameConfigManager::typeInt)
      valueMap->setIntValue(option->index, optionEditFld->getTextUIntValue());
    });
  }

void SettingsDlg::addBoolOption(GameContext* context, UIPanel* parent, uint yOffset, const GameConfigManager::ConfigOption* option)
  {
  addOptionLabel(context, parent, yOffset, option);

  bool checked = false;
  configValueMap->getBoolValue(option->index, &checked);

  std::shared_ptr<UICheckButton> toggleButton(new UICheckButton(context->getUIManager()->getNextComponentID(), checked));
  toggleButton->setHorizontalAlignment(alignmentEnd);
  toggleButton->setSize(Vector2D(30, 30));
  toggleButton->setOffset(Vector2D(-20, yOffset));
  toggleButton->setColour(Vector3D(0.5));
  toggleButton->setButtonColour(Vector3D(0.6, 0.3, 0.1), Vector3D(0.2));
  parent->addChild(toggleButton);

  updateValueMapFuncs.emplace_back([toggleButton, option](GameConfigValueMap* valueMap)
    {
    if(option->type == GameConfigManager::typeBool)
     valueMap->setBoolValue(option->index, toggleButton->isToggledDown());
    });
  }

void SettingsDlg::onEscapePressed(GameContext* context)
  {
  onCancelPressed();
  }
