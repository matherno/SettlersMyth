#pragma once

#include "SettlerBehaviourBase.h"

/*
*   
*/

class SettlerManufactureBehaviour : public SettlerBehaviourBase
  {
private:
  bool startedProcess = false;
  Timer processTimer;

public:
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;
  };