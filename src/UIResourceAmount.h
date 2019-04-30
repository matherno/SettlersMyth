#pragma once

#include "UISystem/UIPanel.h"
#include "GameSystem/GameSystem.h"

class UIText;
class ResourceStorage;

class UIResourceAmount : public UIPanel
  {
private:
  string iconFilePath;
  int resAmount = 0;
  std::shared_ptr<UIText> uiAmountText;
  std::function<void(bool)> onResourceToggledFunc;
  bool startToggledOn = false;

public:
  UIResourceAmount(uint id, string iconFilePath, int amount = 0);

  void updateAmount(int amount);
  void setOnResourceToggledFunc(std::function<void(bool toggledOn)> func, bool startToggledOn);

  virtual void initialise(GameContext* context) override;
  };


class UIResourceAmountList : public UIPanel
  {
private:
  struct ResAmountItem
    {
    uint resID = 0;
    string iconFilePath;
    bool startToggledOn = false;
    std::shared_ptr<UIResourceAmount> uiResAmount;
    ResAmountItem() {}
    };

  std::vector<ResAmountItem> resItems;
  ResourceStorage* resourceStorage;
  std::function<void(bool, uint)> onResourceToggledFunc;
  int columnCount = 1;

public:
  UIResourceAmountList(uint id, ResourceStorage* resStorage, int columnCount = 1);

  void addResource(uint resID, string iconFilePath, bool toggledOn = false);
  void addResource(uint resID, GameContext* gameContext, bool toggledOn = false);
  void updateResources();
  void setOnResourceToggledFunc(std::function<void(bool toggledOn, uint resID)> func);

  int getPixelHeightUsed() const;

  virtual void initialise(GameContext* context) override;

protected:
  int getResCount(uint resID) const;
  };