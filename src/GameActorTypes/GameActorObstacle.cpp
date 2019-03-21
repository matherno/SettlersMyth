//
// Created by matt on 24/02/19.
//

#include "GameActorObstacle.h"


SMGameActor* GameActorObstacleBlueprint::doConstructActor(uint actorID) const
  {
  return new GameActorObstacle(actorID, id);
  }


GameActorObstacle::GameActorObstacle(uint id, uint typeID) : SMGameActor(id, typeID)
  {}
