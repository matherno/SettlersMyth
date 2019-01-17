#pragma once

#include "SettlerBehaviourBase.h"

/*
*   
*/

class SettlerCollectResourceBehaviour : public SettlerBehaviourBase
  {
private:
  std::weak_ptr<SMStaticActor> targetBuilding;
  ResourceLock collectResLock;    // lock on the resource this settler is heading to collect
  ResourceReserve collectResSpaceReserve;   //  reservation on the resource space that this settler is going to take the resource too

public:
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;

protected:
  Building* getTargetBuilding();
  virtual void onCancelBehaviour(SMGameActor* gameActor, GameContext* gameContext) override;
  };
