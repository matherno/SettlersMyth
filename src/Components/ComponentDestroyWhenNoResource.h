#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>

/*
*   
*/


class ComponentDestroyWhenNoResourceBlueprint : public SMComponentBlueprint
  {
public:
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class ComponentDestroyWhenNoResource : public SMComponent
  {
private:
  Timer updateTimer;

public:
  ComponentDestroyWhenNoResource(const SMGameActorPtr& actor, SMComponentType type);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;

protected:
  void refreshResourceStackActors(GameContext* gameContext, uint stackIdx);
  };
