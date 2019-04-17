#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>

/*
*   
*/


class ComponentResidentsBlueprint : public SMComponentBlueprint
  {
public:
  string unitBlueprintName;
  uint unitBlueprintID = 0;
  uint maxUnits = 10;

  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class ComponentResidents : public BuildingComponent
  {
private:
  const ComponentResidentsBlueprint* blueprint;

public:
  ComponentResidents(const SMGameActorPtr& actor, SMComponentType type, const ComponentResidentsBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent) override;
  virtual void onMessage(GameContext* gameContext, SMMessage message, void* extra) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;

protected:
  void populateResidents(GameContext* gameContext);
  };
