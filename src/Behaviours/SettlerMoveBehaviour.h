#pragma once

#include "SettlerBehaviourBase.h"

/*
*   
*/

class SettlerMoveBehaviour : public SettlerBehaviourBase
  {
private:
  bool transferResourcesToBase = false;
  GridXY targetFaceDirection;   //  will rotate settler to face this direction when reached target
  std::list<ResourceReserve> baseResSpaceReserves;   //  reservations on the resource space that this settler is going to take the resource too

public:
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;

protected:
  virtual void freeReservedResSpace(SMGameActor* gameActor, GameContext* gameContext);
  virtual void onCancelBehaviour(SMGameActor* gameActor, GameContext* gameContext) override;
  };
