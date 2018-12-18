#pragma once

#include <SMGameActor.h>
#include <Building.h>
#include <GameSystem/Timer.h>
#include "SettlerBehaviourBase.h"

/*
*   
*/

class SettlerHarvestBehaviour : public SettlerBehaviourBase
  {
private:
  std::weak_ptr<SMStaticActor> targetActor;

public:
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;

protected:
  SMStaticActor* getTargetActor();
  };
