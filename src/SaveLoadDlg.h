#pragma once

#include <UISystem/UIPanel.h>
#include <UISystem/UIList.h>
#include <UISystem/UIEditText.h>

/*
*   
*/

typedef std::function<void(string)> OnSavingFunc;
typedef std::function<void(string)> OnLoadingFunc;
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
  OnSavingFunc funcOnSave;
  OnLoadingFunc funcOnLoad;
  OnCancelledFunc funcOnCancel;

public:
  SaveLoadDlg(uint id, Mode mode);
  void setSaveGameStateCallback(OnSavingFunc func) { funcOnSave = func; }
  void setLoadGameStateCallback(OnLoadingFunc func) { funcOnLoad = func; }
  void setCancelledCallback(OnCancelledFunc func) { funcOnCancel = func; }
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
