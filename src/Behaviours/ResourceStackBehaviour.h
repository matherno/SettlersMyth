#pragma once

#include <SMGameObjectFactory.h>

/*
*   
*/

class ResourceStackBehaviour : public IGameObjectBehaviour
  {
private:
  struct ResourceStack
    {
    uint id = 0;
    uint amount = 0;
    std::list<SMGameActorPtr> resourceActors;
    GridXY pos;
    ResourceStack() {}
    ResourceStack(GridXY pos) : pos(pos) {}
    };

  bool carryingResource = false;
  std::vector<ResourceStack> resourceStacks;

public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;

protected:
  void refreshResourceStackActors(SMGameActor* gameActor, GameContext* gameContext, uint stackIdx);
  };
