#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>

/*
*   
*/


class ComponentInitResourcesBlueprint : public SMComponentBlueprint
  {
private:
  std::map<string, uint> initialResourcesByName;
public:
  std::map<uint, uint> initialResources;
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  };


class ComponentInitResources : public SMComponent
  {
private:
  const ComponentInitResourcesBlueprint* blueprint;

public:
  ComponentInitResources(const SMGameActorPtr& actor, SMComponentType type, const ComponentInitResourcesBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent) override;

protected:
  void refreshResourceStackActors(GameContext* gameContext, uint stackIdx);
  };
