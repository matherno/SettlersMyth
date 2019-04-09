#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>
#include "GameActorTypes/GameActorDeposit.h"

/*
*   
*/


class ComponentConstructorBlueprint : public SMComponentBlueprint
  {
private:
  string constructionPackName;

public:
  uint constructionPackID = 0;
  uint constructionTime = 5000;

  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };



class ConstructorUnitCommand : public UnitCommand
  {
private:
  const uint underConstructBuildingID;
  const int constructTime;
  ResourceReserve buildingResReserve;
  Timer constructionTimer;
  enum
    {
    MOVING_TO_BUILDING,
    CONSTRUCTING
    } state;

public:
  ConstructorUnitCommand(uint underConstructBuildingID, int constructTime);

protected:
  virtual void onStart(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onUpdate(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled) override;
  virtual void onReachedTarget(GameContext* gameContext, GameActorUnit* unit) override;

  GameActorBuilding* getUnderConstructionBuilding(GameContext* gameContext) const;
  };



class ComponentConstructor : public BuildingComponent
  {
private:
  const ComponentConstructorBlueprint* blueprint;
  Timer sendUnitTimer;

public:
  ComponentConstructor(const SMGameActorPtr& actor, SMComponentType type, const ComponentConstructorBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;

protected:
  void sendUnitToConstruct(GameContext* gameContext);
  };
