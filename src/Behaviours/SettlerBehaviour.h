#pragma once

#include <SMGameActor.h>
#include <Building.h>
#include <GameSystem/Timer.h>

/*
*   
*/

class SettlerBehaviour : public IGameObjectBehaviour
  {
private:
  uint activeCommand = CMD_IDLE;
  SMStaticActorPtr targetActor;
  Timer timer;

public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual bool processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command) override;

protected:
  Building* getAttachedBuilding(SMGameActor* gameActor, GameContext* gameContext);

  bool startHarvesting(SMGameActor* gameActor, GameContext* gameContext);
  bool updateHarvesting(SMGameActor* gameActor, GameContext* gameContext);

  bool startReturnToBase(SMGameActor* gameActor, GameContext* gameContext);
  bool updateReturnToBase(SMGameActor* gameActor, GameContext* gameContext);
  };
