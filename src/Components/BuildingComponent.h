#pragma once

#include "SMGameActor.h"
#include "GameActorTypes/GameActorBuilding.h"


class BuildingComponent : public SMComponent
  {
protected:
  BuildingComponent(const SMGameActorPtr& actor, SMComponentType type);
  
  GameActorBuilding* getBuildingActor();
  const GameActorBuilding* getBuildingActor() const;
  };