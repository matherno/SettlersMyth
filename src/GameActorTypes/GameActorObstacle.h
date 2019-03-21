#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>

/*
*   
*/



class GameActorObstacleBlueprint : public SMGameActorBlueprint
  {
public:
protected:
  virtual SMGameActor* doConstructActor(uint actorID) const override;
  };



class GameActorObstacle : public SMGameActor
  {
public:
  GameActorObstacle(uint id, uint typeID);

  static GameActorObstacle* cast(SMGameActor* actor) { return dynamic_cast<GameActorObstacle*>(actor); }   
  };
