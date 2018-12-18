#pragma once

#include "SettlerBehaviourBase.h"

/*
*   
*/

class SettlerCollectResourceBehaviour : public SettlerBehaviourBase
  {
private:
  std::weak_ptr<SMStaticActor> targetBuilding;
  uint resToCollect = 0;

public:
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;

  SMStaticActor* getTargetBuilding();

  };
