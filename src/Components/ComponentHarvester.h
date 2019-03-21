#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>
#include "GameActorTypes/GameActorDeposit.h"

/*
*   
*/


class ComponentHarvesterBlueprint : public SMComponentBlueprint
  {
public:
  string depositName;
  uint depositBlueprintID = 0;
  int harvestTime = 1000;

  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class HarvestUnitCommand : public UnitCommand
  {
private:
  const uint depositActorID;
  const int harvestTime;
  ResourceReserve buildingResReserve;
  ResourceLock depositResLock;
  Timer harvestTimer;
  enum
    {
    MOVING_TO_DEPOSIT,
    HARVESTING_DEPOSIT
    } state;

public:
  HarvestUnitCommand(uint depositActorID, int harvestTime);

protected:
  virtual void onStart(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onUpdate(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled) override;
  virtual void onReachedTarget(GameContext* gameContext, GameActorUnit* unit) override;

  GameActorDeposit* getResourceDeposit(GameContext* gameContext) const;
  };


class ComponentHarvester : public BuildingComponent
  {
private:
  const ComponentHarvesterBlueprint* blueprint;
  Timer sendUnitTimer;
  uint resourceBlueprintID = 0;

public:
  ComponentHarvester(const SMGameActorPtr& actor, SMComponentType type, const ComponentHarvesterBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;
  virtual void onMessage(GameContext* gameContext, SMMessage message, void* extra) override;
  };
