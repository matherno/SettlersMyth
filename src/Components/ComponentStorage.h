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

class UIResourceAmountList;

class ComponentStorage : public BuildingComponent
  {
private:
  const ComponentStorageBlueprint* blueprint;
  
  Timer collectResTimer;
  std::map<uint, bool> resourcesCanCollect;
  std::vector<uint> resCollectCycle;
  int resCollectIdx = 0;
  std::shared_ptr<UIResourceAmountList> uiResourceList;

public:
  ComponentStorage(const SMGameActorPtr& actor, SMComponentType type, const ComponentStorageBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;
  virtual int onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset) override;
  virtual void onUpdateSelectionHUD(GameContext* gameContext) override;
  virtual void save(GameContext* gameContext, XMLElement* xmlComponent) override;

  bool gotEnoughInputResources() const;

protected:
  void sendUnitToCollect(GameContext* gameContext);
  void sendUnitToManufacture(GameContext* gameContext);
  void resetResCollectCycleArray();
  };
