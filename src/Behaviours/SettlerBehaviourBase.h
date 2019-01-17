#pragma once

#include <SMGameObjectFactory.h>
#include <TowOff/GameSystem/Timer.h>
#include <Building.h>

/*
*   
*/

class SettlerBehaviourBase : public IGameObjectBehaviour
  {
private:
  bool performingCommand = false;
  uint activeCommand = 0;

protected:
  Timer timer;

public:
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override {}
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override {}

protected:
  Building* getAttachedBuilding(SMGameActor* gameActor, GameContext* gameContext);
  virtual void onCancelBehaviour(SMGameActor* gameActor, GameContext* gameContext) {};
  virtual void startBehaviour(SMGameActor* gameActor, GameContext* gameContext, uint commandID);
  virtual void endBehaviour(SMGameActor* gameActor, GameContext* gameContext, bool returnToBase = false);
  virtual bool isBehaviourActive() const { return performingCommand; };
  };
