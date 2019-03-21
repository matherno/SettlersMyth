#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>
#include "GridMapHandler.h"
#include "GameActorBuilding.h"


/*
 *
 */

class GameActorBuilding;

class GameActorUnitBlueprint : public SMGameActorBlueprint
  {
public:
  double speed = 1;

protected:
  virtual bool loadFromXML(XMLElement* xmlElement, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMGameActor *doConstructActor(uint actorID) const override;
  };


class GameActorUnit;

class UnitCommand
  {
private:
  bool commandRunning = false;
  bool gotTarget = false;
  Vector2D targetPosition;
  double targetRotation = 0;
  std::unique_ptr<GridMapPath> pathToTarget;

public:
  void startCommand(GameContext* gameContext, GameActorUnit* unit);
  void updateCommand(GameContext* gameContext, GameActorUnit* unit);
  void cancelCommand(GameContext* gameContext, GameActorUnit* unit);
  void endCommand(GameContext* gameContext, GameActorUnit* unit);
  bool isCommandEnded();

protected:
  void setTargetPosition(const Vector2D& position, double rotation = 0);
  void setTargetPosition(GridXY position, double rotation = 0);
  void clearTarget();

  virtual void onStart(GameContext* gameContext, GameActorUnit* unit) {}
  virtual void onUpdate(GameContext* gameContext, GameActorUnit* unit) {}
  virtual void onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled) {}
  virtual void onReachedTarget(GameContext* gameContext, GameActorUnit* unit) {}
  virtual void onCantReachTarget(GameContext* gameContext, GameActorUnit* unit) { cancelCommand(gameContext, unit); }

private:
  void moveToTarget(GameContext* gameContext, GameActorUnit* unit);
  };
typedef std::shared_ptr<UnitCommand> UnitCommandPtr;



class ReturnToBaseUnitCommand : public UnitCommand
  {
private:
  ResourceReserve resourceToDropOff;

public:
  ReturnToBaseUnitCommand(ResourceReserve resourceToDropOff);
  ReturnToBaseUnitCommand();

protected:
  virtual void onStart(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onReachedTarget(GameContext* gameContext, GameActorUnit* unit) override;
  };


class CollectResourceUnitCommand : public UnitCommand
  {
private:
  ResourceReserve attachedBuildingResReserve;
  ResourceLock collectionResLock;
  const uint actorIDToCollectFrom;
  const uint resourceID;

public:
  CollectResourceUnitCommand(uint actorIDToCollectFrom, uint resourceID);

protected:
  virtual void onStart(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onReachedTarget(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled) override;
  };



class GameActorUnit : public SMGameActor
  {
private:
  friend class GameActorUnitBlueprint;
  uint attachedBuilding = 0;
  double speed = 1;   // units per second
  UnitCommandPtr currentCommand;

public:
  GameActorUnit(uint id, uint typeID);

  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;

  void performCommand(UnitCommandPtr command);
  void returnToAttachedBuilding(GameContext* gameContext, ResourceReserve resourceToDropOff = ResourceReserve());
  bool isIdling() const;
  double getMovementSpeed() const { return speed; }

  void setAttachedBuilding(uint id) { attachedBuilding = id; }
  void detachFromBuilding() { attachedBuilding = 0; }
  uint getAttachedBuilding() const { return attachedBuilding; }
  GameActorBuilding* getAttachedBuilding(GameContext* gameContext) const;
  bool isAttachedToBuilding() const { return attachedBuilding > 0; }

  static GameActorUnit* cast(SMGameActor* actor) { return dynamic_cast<GameActorUnit*>(actor); }
  };

typedef std::shared_ptr<GameActorUnit> GameActorUnitPtr;
