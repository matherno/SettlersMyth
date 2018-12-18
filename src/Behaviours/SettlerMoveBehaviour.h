#pragma once

#include "SettlerBehaviourBase.h"

/*
*   
*/

class SettlerMoveBehaviour : public SettlerBehaviourBase
  {
private:
  bool transferResourcesToBase = false;

public:
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;
  };
