#pragma once

#include <UISystem/UIPanel.h>
#include <UISystem/UIList.h>
#include <UISystem/UIEditText.h>
#include "SMGameSaveLoad.h"

/*
*   
*/

typedef std::function<std::shared_ptr<TOGameState>()> OnSavingFunc;
typedef std::function<void(std::shared_ptr<TOGameState> state)> OnLoadingFunc;
typedef std::function<void()> OnCancelledFunc;

class SaveLoadDlg : public UIPanel
  {
public:
  enum Mode
    {
    modeSave,
    modeLoad,
    };

private:
  const Mode mode;
  std::shared_ptr<UIList> listComponent;
  std::shared_ptr<UIEditText> saveNameTextComp;
  std::map<uint, string> saveFiles;
  OnSavingFunc onSavingFunc;
  OnLoadingFunc onLoadingFunc;
  OnCancelledFunc onCancelledFunc;

public:
  SaveLoadDlg(uint id, Mode mode);
  void setSaveGameStateCallback(OnSavingFunc func) { onSavingFunc = func; }
  void setLoadGameStateCallback(OnLoadingFunc func) { onLoadingFunc = func; }
  void setCancelledCallback(OnCancelledFunc func) { onCancelledFunc = func; }
  virtual void initialise(GameContext* context) override;
  virtual void onEscapePressed(GameContext* context) override;
  static void getSavedFiles(std::list<string>* files);

protected:
  void buildSaveFileList();
  void onDeletePressed(GameContext* context);
  void onLoadPressed();
  void onSavePressed(GameContext* context);
  void onSaveNewPressed();
  void onCancelPressed();
  string getSelectedFilePath();
  string getNewSaveFilePath();
  };
