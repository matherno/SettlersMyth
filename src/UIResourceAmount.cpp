#include "UIResourceAmount.h"
#include "UISystem/UIText.h"
#include "Utils.h"
#include "SMGameContext.h"
#include "ResourceStorage.h"

static const int iconSize = 32;
static const int iconMargin = 10;
static const int fontSize = 25;
static const int resAmountWidth = iconSize + fontSize + iconMargin;
static const int resAmountHeight = std::max(iconSize, fontSize);

UIResourceAmount::UIResourceAmount(uint id, string iconFilePath, int amount) 
  : UIPanel(id), iconFilePath(iconFilePath), resAmount(amount)
  {
  setSize(Vector2D(resAmountWidth, resAmountHeight));
  setColour(HUD_COL_BG);
  }

void UIResourceAmount::updateAmount(int amount)
  {
  if (resAmount != amount)
    {
    resAmount = amount;
    if (uiAmountText)
      {
      uiAmountText->setText(std::to_string(resAmount));
      uiAmountText->invalidate();
      }
    }
  }

void UIResourceAmount::setOnResourceToggledFunc(std::function<void(bool toggledOn)> func, bool startToggledOn)
  {
  onResourceToggledFunc = func;
  this->startToggledOn = startToggledOn;
  }

void UIResourceAmount::initialise(GameContext* gameContext)
  {
  UIManager* uiManager = gameContext->getUIManager();
  
  UIPanel* icon = nullptr;
  if (onResourceToggledFunc)
    {
    UIButton* btn = new UIButton(uiManager->getNextComponentID(), true, startToggledOn);
    btn->setMouseClickCallback([btn, this](uint, uint)
      {
      onResourceToggledFunc(btn->isToggledDown());  
      return true;
      });
    if (!iconFilePath.empty())
      btn->setButtonTexture(gameContext->getRenderContext()->getSharedTexture(iconFilePath));
    btn->setHighlightWidth(BTN_BORDER_SIZE);
    btn->setButtonHighlightColour(colToVec3(90, 90, 100), colToVec3(70, 70, 72));
    btn->setSize(Vector2D(iconSize + BTN_BORDER_SIZE * 2));
    btn->setOffset(Vector2D(-BTN_BORDER_SIZE, 0));
    icon = btn;
    }
  else
    {
    icon = new UIPanel(uiManager->getNextComponentID());
    if (!iconFilePath.empty())
      icon->setTexture(gameContext->getRenderContext()->getSharedTexture(iconFilePath));
    icon->setSize(Vector2D(iconSize));
    icon->setOffset(Vector2D(0, 0));
    }

  icon->setVerticalAlignment(Alignment::alignmentCentre);
  addChild(UIComponentPtr(icon));

  uiAmountText.reset(new UIText(uiManager->getNextComponentID()));
  uiAmountText->setOffset(Vector2D(iconSize + iconMargin, 1));
  uiAmountText->setSize(Vector2D(fontSize, fontSize));
  uiAmountText->setFontSize(fontSize);
  uiAmountText->setFontColour(Vector3D(0));
  uiAmountText->showBackground(false);
  uiAmountText->setTextCentreAligned(true, false);
  uiAmountText->setText(std::to_string(resAmount));
  uiAmountText->setVerticalAlignment(Alignment::alignmentCentre);
  addChild(uiAmountText);

  UIPanel::initialise(gameContext);
  }



static const int paddingBetweenItems = 10;
static const int paddingBetweenColumns = 10;

UIResourceAmountList::UIResourceAmountList(uint id, ResourceStorage* resStorage, int columnCount) 
    : UIPanel(id), resourceStorage(resStorage), columnCount(columnCount)
  {
  setColour(HUD_COL_BG);
  }

void UIResourceAmountList::addResource(uint resID, string iconFilePath, bool toggledOn)
  {
  resItems.emplace_back();
  ResAmountItem& item = resItems.back();
  item.resID = resID;
  item.iconFilePath = iconFilePath;
  item.startToggledOn = toggledOn;
  item.uiResAmount = nullptr;
  }

void UIResourceAmountList::addResource(uint resID, GameContext* gameContext, bool toggledOn)
  {
  const SMGameActorBlueprint* blueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->getGameActorBlueprint(resID);
  ASSERT(blueprint, "")
  if (blueprint)
    addResource(resID, blueprint->iconPath, toggledOn);
  }

void UIResourceAmountList::updateResources()
  {
  for (ResAmountItem& item : resItems)
    {
    if(item.uiResAmount);
      item.uiResAmount->updateAmount(getResCount(item.resID));
    }
  }

void UIResourceAmountList::setOnResourceToggledFunc(std::function<void(bool toggledOn, uint resID)> func)
  {
  onResourceToggledFunc = func;
  }


int UIResourceAmountList::getPixelHeightUsed() const
  {
  const int itemCount = resItems.size();
  return itemCount * resAmountHeight + ceil(itemCount / columnCount) * paddingBetweenItems;
  }

void UIResourceAmountList::initialise(GameContext* gameContext)
  {
  UIManager* uiManager = gameContext->getUIManager();
  
  int index = 0;
  for (ResAmountItem& item : resItems)
    {
    std::shared_ptr<UIResourceAmount> resAmount(new UIResourceAmount(uiManager->getNextComponentID(), item.iconFilePath, getResCount(item.resID)));

    const int column = index % columnCount;
    const int row = index / columnCount;
    resAmount->setOffset(Vector2D(
      column * (resAmountWidth + paddingBetweenColumns), 
      row * (resAmountHeight + paddingBetweenItems)
      ));
    ++index;

    if (onResourceToggledFunc)
      {
      int resID = item.resID;
      resAmount->setOnResourceToggledFunc([resID, this](bool toggledOn)
        {
        onResourceToggledFunc(toggledOn, resID);
        }, item.startToggledOn);
      }

    item.uiResAmount = resAmount;
    addChild(resAmount);
    }

  setSize(Vector2D(columnCount * resAmountWidth + (columnCount-1) * paddingBetweenColumns, 0));

  UIPanel::initialise(gameContext);
  }

int UIResourceAmountList::getResCount(uint resID) const
  {
  return resourceStorage->resourceCount(resID, true, true);
  }
