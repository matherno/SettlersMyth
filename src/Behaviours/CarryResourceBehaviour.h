#pragma once

#include <SMGameObjectFactory.h>

/*
*   
*/

class CarryResourceBehaviour : public IGameObjectBehaviour
  {
private:
  bool carryingResource = false;
  SMGameActorWkPtr resourceActor;

public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;

protected:
  void createResourceActor(SMGameActor* gameActor, GameContext* gameContext, uint gameObjDefID);
  void updateResourceActorTransform(SMGameActor* gameActor);
  };
