#pragma once

#include <SMGameObjectFactory.h>
#include <TowOff/GameSystem/Timer.h>

/*
*
*/

class HarvesterBehaviour : public IGameObjectBehaviour
  {
private:
  Timer sendUnitTimer;
  uint resourceID = 0;

public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;
  };
