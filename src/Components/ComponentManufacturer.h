#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>
#include "GameActorTypes/GameActorDeposit.h"

/*
*   
*/


class ComponentManufacturerBlueprint : public SMComponentBlueprint
  {
private:
  std::map<string, uint> inputsByName;
  string outputResName;

public:
  std::map<uint, uint> inputs;
  uint outputResID = 0;
  uint outputResAmount = 0;
  uint manufactureTime = 0;
  GridXY manufactureSpot;
  GridXY manufactureDir;
  bool keepOutput = false;  //  if true, then units from other buildings can not take output resource from this

  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class ManufactureUnitCommand : public UnitCommand
  {
private:
  const ComponentManufacturerBlueprint* manufactureBlueprint;
  Timer processTimer;
  ResourceReserve outputResReserve;
  enum
    {
    MOVING_TO_MANUFACTURE,
    MANUFACTURING
    } state;

public:
  ManufactureUnitCommand(const ComponentManufacturerBlueprint* manufactureBlueprint);

protected:
  virtual void onStart(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onUpdate(GameContext* gameContext, GameActorUnit* unit) override;
  virtual void onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled) override;
  virtual void onReachedTarget(GameContext* gameContext, GameActorUnit* unit) override;
  };


class ComponentManufacturer : public BuildingComponent
  {
private:
  const ComponentManufacturerBlueprint* blueprint;
  
  Timer collectResTimer;
  std::vector<uint> resCollectCycle;
  int resCollectIdx = 0;

  Timer startManufactureTimer;
  bool isUnitManufacturing = false;   //  a unit is currently in the process of manufacturing (either moving to or performing)

public:
  ComponentManufacturer(const SMGameActorPtr& actor, SMComponentType type, const ComponentManufacturerBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;
  virtual void onMessage(GameContext* gameContext, SMMessage message, void* extra) override;

  bool gotEnoughInputResources() const;

protected:
  void sendUnitToCollect(GameContext* gameContext);
  void sendUnitToManufacture(GameContext* gameContext);
  };
