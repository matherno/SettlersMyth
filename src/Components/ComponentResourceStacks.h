#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>

/*
*   
*/


class ComponentResourceStacksBlueprint : public SMComponentBlueprint
  {
public:
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class ComponentResourceStacks : public BuildingComponent
  {
private:
  struct Stack
    {
    uint id = 0;
    uint amount = 0;
    std::list<SMGameActorPtr> resourceActors;
    GridXY pos;
    Stack() {}
    Stack(GridXY pos) : pos(pos) {}
    };

  std::vector<Stack> resourceStacks;
  Timer updateTimer;

public:
  ComponentResourceStacks(const SMGameActorPtr& actor, SMComponentType type);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;

protected:
  void refreshResourceStackActors(GameContext* gameContext, uint stackIdx);
  };
