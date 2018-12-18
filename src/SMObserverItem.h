#pragma once

#include <TowOff/GameSystem/GameSystem.h>

/*
*   
*/

class SMObserverItem;
typedef std::shared_ptr<SMObserverItem> SMObserverItemPtr;
typedef std::weak_ptr<SMObserverItem> SMObserverItemWkPtr;

class SMObserverItem
  {
private:
  std::vector<SMObserverItemWkPtr> observedItems;   // items this is watching
  std::vector<SMObserverItem*> observers;           // items that are watching this

public:
  SMObserverItem();
  virtual ~SMObserverItem();

  void startObservingItem(SMObserverItemPtr itemToObserve);
  void stopObservingItem(SMObserverItemPtr itemObserving);

  void notifyObserversCommandStart(GameContext* gameContext, uint cmdID);
  virtual void notifyObserversCommandFinish(GameContext* gameContext, uint cmdID);
  virtual void notifyObserversDestroyed(GameContext* gameContext);

protected:
  virtual void observedCommandStart(GameContext* gameContext, SMObserverItem* observed, uint cmdID) {};
  virtual void observedCommandFinish(GameContext* gameContext, SMObserverItem* observed, uint cmdID) {};
  virtual void observedDestroyed(GameContext* gameContext, SMObserverItem* observed) {};

private:
  void addObserver(SMObserverItem* observerActor);
  void removeObserver(SMObserverItem* observerActor);
  };
