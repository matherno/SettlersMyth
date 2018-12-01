//
// Created by matt on 1/12/18.
//

#include "Unit.h"

Unit::Unit(uint id, const IGameObjectDef* gameObjectDef) : SMDynamicActor(id, gameObjectDef)
  {
  }

void Unit::onUpdate(GameContext* gameContext)
  {
  auto topOrder = getTopOrder();
  if (hasStartedOrder)
    {
    topOrder->update(this, gameContext);
    if (topOrder->isDone(this, gameContext))
      {
      orderList.pop_front();
      hasStartedOrder = false;
      }
    }
  else
    {
    if (topOrder)
      {
      topOrder->start(this, gameContext);
      hasStartedOrder = true;
      }
    }
  SMDynamicActor::onUpdate(gameContext);
  }

void Unit::onDetached(GameContext* gameContext)
  {
  clearOrders(gameContext);
  SMGameActor::onDetached(gameContext);
  }

void Unit::pushOrder(IUnitOrderPtr order)
  {
  orderList.push_back(order);
  }

void Unit::clearOrders(GameContext* gameContext)
  {
  if (!isIdling() && getOrderCount() > 0)
    getTopOrder()->cancel(this, gameContext);
  orderList.clear();
  hasStartedOrder = false;
  }

IUnitOrderPtr Unit::getTopOrder()
  {
  if (getOrderCount() > 0)
    return orderList.front();
  return IUnitOrderPtr();
  }

