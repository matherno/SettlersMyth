#pragma once

#include "Unit.h"

/*
*   
*/

class Unit;

class IUnitOrder
  {
public:
  virtual void start(Unit* unit, GameContext* context) = 0;
  virtual void update(Unit* unit, GameContext* context) = 0;
  virtual void cancel(Unit* unit, GameContext* context) = 0;
  virtual bool isDone(Unit* unit, GameContext* context) = 0;
  };
typedef std::shared_ptr<IUnitOrder> IUnitOrderPtr;


class UnitOrderMoveToTarget : public IUnitOrder
  {
private:
  Vector2D target;
  bool endOrder = false;

public:
  UnitOrderMoveToTarget(Vector2D target) : target(target) {}

  virtual void start(Unit* unit, GameContext* context) override;
  virtual void update(Unit* unit, GameContext* context) override;
  virtual void cancel(Unit* unit, GameContext* context) override;
  virtual bool isDone(Unit* unit, GameContext* context) override;
  };