//
// Created by matt on 30/03/18.
//

#include <UISystem/UIMessageBox.h>
#include "SaveLoadDlg.h"
#include "SaveLoadFileHelper.h"

#define MAX_SAVES 9   // only limited by what can fit on the listComponent at one time (need pagination/scrolling)


SaveLoadDlg::SaveLoadDlg(uint id, Mode mode) : UIPanel(id), mode(mode)
  {
  }

void SaveLoadDlg::initialise(GameContext* context)
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

  UIPanel* listBG = new UIList(uiManager->getNextComponentID());
  listBG->setOffset(Vector2D(0, 40));
  listBG->setSize(Vector2D(730, 450));
  listBG->setHorizontalAlignment(Alignment::alignmentCentre);
  listBG->setColour(Vector3D(0.35, 0.25, 0.15));
  subPanel->addChild(UIComponentPtr(listBG));

  buildSaveFileList();
  UIList* list = new UIList(uiManager->getNextComponentID());
  list->setVerticalAlignment(Alignment::alignmentCentre);
  list->setHorizontalAlignment(Alignment::alignmentCentre);
  list->setWidthMatchParent(true);
  list->setHeightMatchParent(true);
  list->setPadding(5, 5);
  list->setColour(Vector3D(0.4, 0.3, 0.2));
  for (auto pair : saveFiles)
    list->addItem(pair.first, mathernogl::removeFileExtension(pair.second));
  list->setItemHeight(45);
  list->setItemGap(0);
  list->setItemColour(Vector3D(0.4, 0.3, 0.2));
  list->setItemSelectColour(Vector3D(0.3, 0.5, 0.2));
  list->setItemTextColour(Vector3D(0.1));
  list->setItemDblClickedCallback([this, context](uint id)
    {
    if(mode == modeSave)
      onSavePressed(context);
    else
      onLoadPressed();
    });
  listComponent.reset(list);
  listBG->addChild(listComponent);

  if (mode == modeSave)
    {
    UIPanel* editTextBG = new UIList(uiManager->getNextComponentID());
    editTextBG->setSize(Vector2D(500, 35));
    editTextBG->setOffset(Vector2D(60, -80));
    editTextBG->setVerticalAlignment(Alignment::alignmentEnd);
    editTextBG->setColour(Vector3D(0.35, 0.25, 0.15));
    subPanel->addChild(UIComponentPtr(editTextBG));

    saveNameTextComp.reset(new UIEditText(uiManager->getNextComponentID()));
    saveNameTextComp->setText("New Save");
    saveNameTextComp->setFontSize(25);
    saveNameTextComp->setColour(Vector3D(0.5, 0.4, 0.3));
    saveNameTextComp->setTextCentreAligned(true, false);
    saveNameTextComp->setWidthMatchParent(true);
    saveNameTextComp->setHeightMatchParent(true);
    saveNameTextComp->setPadding(3, 3);
    saveNameTextComp->setCharacterLimit(MAX_SAVE_NAME_LENGTH);
    saveNameTextComp->setEditType(UIEditText::typeAlphaNumeric);
    editTextBG->addChild(saveNameTextComp);
    }

  float buttonOffsetX = -40;
  auto createButton = [uiManager, subPanel, &buttonOffsetX](string text, OnMouseClickCallback mouseClickCallback)
    {
    static const Vector2D buttonSize(100, 30);
    UIButton* saveLoadButton = new UIButton(uiManager->getNextComponentID(), false);
    saveLoadButton->setSize(buttonSize);
    saveLoadButton->setOffset(Vector2D(buttonOffsetX, -25));
    saveLoadButton->setButtonColour(Vector3D(0.5, 0.3, 0.1));
    saveLoadButton->setButtonText(text, Vector3D(0), Vector3D(0.40, 0.20, 0.05), 25);
    saveLoadButton->setHighlightWidth(2);
    saveLoadButton->setVerticalAlignment(Alignment::alignmentEnd);
    saveLoadButton->setHorizontalAlignment(Alignment::alignmentEnd);
    saveLoadButton->setButtonHighlightColour(Vector3D(0.40, 0.20, 0.05), Vector3D(0.45, 0.25, 0.07));
    saveLoadButton->setMouseClickCallback(mouseClickCallback);
    subPanel->addChild(UIComponentPtr(saveLoadButton));
    buttonOffsetX -= 25 + buttonSize.x;
    };

  createButton("Cancel", [this](uint mouseX, uint mouseY){ onCancelPressed(); return true; });
  createButton("Delete", [this, context](uint mouseX, uint mouseY){ onDeletePressed(context); return true; });
  if (mode == modeLoad)
    {
    createButton("Load", [this](uint mouseX, uint mouseY){ onLoadPressed(); return true; });
    }
  else // modeSave
    {
    createButton("Save", [this, context](uint mouseX, uint mouseY){ onSavePressed(context); return true; });
    createButton("Save New", [this](uint mouseX, uint mouseY){ onSaveNewPressed(); return true; });
    }
  }

void SaveLoadDlg::getSavedFiles(std::list<string>* files)
  {
  std::list<string> dirFiles;
  mathernogl::getFilesInDirectory(SAVES_DIRECTORY, &dirFiles, SAVES_FILE_EXT);
  for (string file : dirFiles)
    {
    files->push_back(file);
    }
  }

void SaveLoadDlg::buildSaveFileList()
  {
  std::list<string> files;
  getSavedFiles(&files);
  uint id = 0;
  for (string file : files)
    {
    saveFiles[id] = file;
    ++id;
    if (id >= MAX_SAVES)
      break;
    }
  }

void SaveLoadDlg::onDeletePressed(GameContext* context)
  {
  uint selected;
  if(!listComponent->getSelectedItem(&selected))
    return;

  UIMessageBox::popupMessageBox(context->getUIManager(), "Delete save?", UIMessageBox::modeContinueCancel,
    [this, context, selected](UIMessageBox::MsgBoxResult result)
      {
      if (result == UIMessageBox::resultContinue)
        {
        if(remove(getSelectedFilePath().c_str()) == 0)
          {
          listComponent->removeItem(selected);
          saveFiles.erase(selected);
          listComponent->invalidate();
          }
        }
      });
  }

void SaveLoadDlg::onLoadPressed()
  {
  string filePath = getSelectedFilePath();
  if (funcOnLoad && !filePath.empty())
    funcOnLoad(filePath);
  }

void SaveLoadDlg::onSavePressed(GameContext* context)
  {
  string filePath = getSelectedFilePath();
  if (funcOnSave && !filePath.empty())
    {
    UIMessageBox::popupMessageBox(context->getUIManager(), "Save over existing save?", UIMessageBox::modeContinueCancel,
      [this, context, filePath](UIMessageBox::MsgBoxResult result)
        {
        if (result == UIMessageBox::resultContinue)
          funcOnSave(filePath);
        });
    }
  }

void SaveLoadDlg::onSaveNewPressed()
  {
  string filePath = getNewSaveFilePath();
  if (funcOnSave && !filePath.empty() && saveFiles.size() < MAX_SAVES)
    funcOnSave(filePath);
  }

void SaveLoadDlg::onCancelPressed()
  {
  if (funcOnCancel)
    funcOnCancel();
  }

string SaveLoadDlg::getSelectedFilePath()
  {
  uint selected;
  if(listComponent->getSelectedItem(&selected))
    return SAVES_DIRECTORY + saveFiles[selected];
  return "";
  }

string SaveLoadDlg::getNewSaveFilePath()
  {
  string filePath = "";
  if (saveNameTextComp)
    {
    filePath = saveNameTextComp->getText();
    if (!filePath.empty())
      return SAVES_DIRECTORY + filePath + "." + SAVES_FILE_EXT;
    }
  return filePath;
  }

void SaveLoadDlg::onEscapePressed(GameContext* context)
  {
  if (funcOnCancel)
    funcOnCancel();
  }
