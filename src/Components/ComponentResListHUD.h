#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>
#include "GameActorTypes/GameActorDeposit.h"

/*
*   
*/

class ComponentResListHUDBlueprint : public SMComponentBlueprint
  {
private:
  std::vector<string> resourcesByName;

public:
  std::vector<uint> resources;
  
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };



class UIResourceAmountList;

class ComponentResListHUD : public SMComponent
  {
private:
  const ComponentResListHUDBlueprint* blueprint;
  std::shared_ptr<UIResourceAmountList> uiResourceList;

public:
  ComponentResListHUD(const SMGameActorPtr& actor, SMComponentType type, const ComponentResListHUDBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;
  virtual int onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset) override;
  virtual void onUpdateSelectionHUD(GameContext* gameContext) override;
  };
