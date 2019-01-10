#pragma once

#include <SMGameObjectFactory.h>
#include <TowOff/GameSystem/Timer.h>

/*
*   
*/

class ManufacturerBehaviour : public IGameObjectBehaviour
  {
private:
  Timer sendUnitTimer;
  std::vector<uint> resCollectCycle;
  int resCollectIdx = 0;

  Timer manufactureCooldownTimer;
  uint manufacturingUnitID = 0;
  bool isManufacturing = false;

public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;
protected:
  virtual void observedCommandFinish(GameContext* gameContext, SMObserverItem* observed, uint cmdID) override;
  virtual void observedDestroyed(GameContext* gameContext, SMObserverItem* observed) override;

private:
  uint getNextResourceToCollect(SMGameActor* gameActor);
  bool canStartManufacturing(SMGameActor* gameActor, GameContext* gameContext);
  void resetManufacturing();
  };
