//
// Created by matt on 1/12/18.
//

#include "UnitOrders.h"

void UnitOrderMoveToTarget::start(Unit* unit, GameContext* context)
  {
  unit->setTarget(target);
  }

void UnitOrderMoveToTarget::update(Unit* unit, GameContext* context)
  {
  endOrder = unit->hasReachedTarget();
  }

void UnitOrderMoveToTarget::cancel(Unit* unit, GameContext* context)
  {
  unit->clearTarget();
  }

bool UnitOrderMoveToTarget::isDone(Unit* unit, GameContext* context)
  {
  return endOrder;
  }
