#pragma once

#include <SMGameObjectFactory.h>
#include <TowOff/GameSystem/Timer.h>

/*
*   
*/

class StorageBehaviour : public IGameObjectBehaviour
  {
private:
  Timer sendUnitTimer;
  std::vector<uint> resourcesToStore;
  int nextResToCollect = 0;

public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;
  };
