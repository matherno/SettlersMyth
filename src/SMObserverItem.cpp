//
// Created by matt on 17/12/18.
//

#include "SMObserverItem.h"

SMObserverItem::SMObserverItem()
  {

  }

SMObserverItem::~SMObserverItem()
  {
  for (auto item : observedItems)
    {
    if (item.lock())
      item.lock()->removeObserver(this);
    }
  }

void SMObserverItem::startObservingItem(SMObserverItemPtr itemToObserve)
  {
  auto iter = observedItems.begin();
  while (iter != observedItems.end())
    {
    if (auto item = iter->lock())
      {
      if (item.get() == itemToObserve.get())
        return;   // already observing
      }
    iter++;
    }

  observedItems.push_back(itemToObserve);
  itemToObserve->addObserver(this);
  }


void SMObserverItem::stopObservingItem(SMObserverItemPtr itemObserving)
  {
  auto iter = observedItems.begin();
  while (iter != observedItems.end())
    {
    if (auto item = iter->lock())
      {
      if (item.get() == itemObserving.get())
        {
        observedItems.erase(iter);
        itemObserving->removeObserver(this);
        }
      }
    iter++;
    }
  }

void SMObserverItem::addObserver(SMObserverItem* observerActor)
  {
  observers.push_back(observerActor);
  }

void SMObserverItem::removeObserver(SMObserverItem* observerActor)
  {
  auto iter = observers.begin();
  while (iter != observers.end())
    {
    if (*iter == observerActor)
      {
      observers.erase(iter);
      return;
      }
    iter++;
    }
  }



void SMObserverItem::notifyObserversCommandStart(GameContext* gameContext, uint cmdID)
  {
  for (auto obs : observers)
    obs->observedCommandStart(gameContext, this, cmdID);
  }

void SMObserverItem::notifyObserversCommandFinish(GameContext* gameContext, uint cmdID)
  {
  for (auto obs : observers)
    obs->observedCommandFinish(gameContext, this, cmdID);
  }

void SMObserverItem::notifyObserversDestroyed(GameContext* gameContext)
  {
  for (auto obs : observers)
    obs->observedDestroyed(gameContext, this);
  }

