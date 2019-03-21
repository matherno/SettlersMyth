#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>
#include "GameActorTypes/GameActorDeposit.h"

/*
*   
*/


class ComponentStorageBlueprint : public SMComponentBlueprint
  {
public:
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class ComponentStorage : public BuildingComponent
  {
private:
  const ComponentStorageBlueprint* blueprint;
  
  Timer collectResTimer;
  std::vector<uint> resCollectCycle;
  int resCollectIdx = 0;

  Timer startManufactureTimer;
  bool isUnitManufacturing = false;   //  a unit is currently in the process of manufacturing (either moving to or performing)

public:
  ComponentStorage(const SMGameActorPtr& actor, SMComponentType type, const ComponentStorageBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;

  bool gotEnoughInputResources() const;

protected:
  void sendUnitToCollect(GameContext* gameContext);
  void sendUnitToManufacture(GameContext* gameContext);
  };
