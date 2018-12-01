#pragma once

#include "SMGameActor.h"
#include "UnitOrders.h"

/*
*   
*/

class IUnitOrder;
typedef std::shared_ptr<IUnitOrder> IUnitOrderPtr;

class Unit : public SMDynamicActor
  {
private:
  std::list<IUnitOrderPtr> orderList;
  bool hasStartedOrder = false;

public:
  Unit(uint id, const IGameObjectDef* gameObjectDef);

  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;

  IUnitOrderPtr getTopOrder();
  void pushOrder(IUnitOrderPtr order);
  void clearOrders(GameContext* gameContext);
  bool isIdling() const { return orderList.size() == 0; }
  int getOrderCount() const { return (int)orderList.size(); }

  static Unit* cast(SMGameActor* gameActor){ return dynamic_cast<Unit*>(gameActor); }
  };
