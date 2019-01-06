#pragma once

#include <TowOff/GameSystem/Timer.h>
#include "SMGameActor.h"
#include "GridMapHandler.h"

/*
*   
*/

class IUnitOrder;
typedef std::shared_ptr<IUnitOrder> UnitOrderPtr;

class Unit : public SMDynamicActor
  {
protected:
  bool hasStartedOrder = false;
  uint attachedBuilding = 0;

  bool gotTarget = false;
  bool cantReachTarget = false;
  Vector2D targetPosition;
  std::unique_ptr<GridMapPath> pathToTarget;
  double speed = 1;   // units per second
  bool idling = true;

public:
  Unit(uint id, const IGameObjectDef* gameObjectDef);

  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;

  bool isIdling() const { return idling; }
  void setIdle(bool idle) { idling = idle; }

  void setAttachedBuilding(uint id) { attachedBuilding = id; }
  void detachFromBuilding() { attachedBuilding = 0; }
  uint getAttachedBuilding() const { return attachedBuilding; }
  bool isAttachedToBuilding() const { return attachedBuilding > 0; }

  bool hasGotTarget() const { return gotTarget; }
  bool hasReachedTarget() const;
  bool canNotReachTarget() const;
  void setTarget(Vector2D target);
  void clearTarget();
  void setSpeed(double speed) { this->speed = speed; }
  double getSpeed() const { return speed; }

  static Unit* cast(SMGameActor* gameActor){ return dynamic_cast<Unit*>(gameActor); }
  static Unit* cast(SMObserverItem* obsItem){ return dynamic_cast<Unit*>(obsItem); }
  static std::shared_ptr<Unit> cast(SMGameActorPtr gameActor){ return std::dynamic_pointer_cast<Unit>(gameActor); }
  };

typedef std::shared_ptr<Unit> UnitPtr;
typedef std::weak_ptr<Unit> UnitWkPtr;
