#pragma once

#include <SMGameObjectFactory.h>
#include "TowOff/GameSystem/Timer.h"

/*
*   
*/

class CarryResourceBehaviour : public IGameObjectBehaviour
  {
private:
  bool carryingResource = false;
  SMGameActorWkPtr resourceActor;
  Timer updateTimer;

public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;

protected:
  void createResourceActor(SMGameActor* gameActor, GameContext* gameContext, uint gameObjDefID);
  void updateResourceActorTransform(SMGameActor* gameActor);
  };
