#include "BuildingComponent.h"



BuildingComponent::BuildingComponent(const SMGameActorPtr& actor, SMComponentType type) : SMComponent(actor, type) 
  { 
  ASSERT(getBuildingActor(), "This component can only be attached to a Building Actor") 
  }

GameActorBuilding* BuildingComponent::getBuildingActor()
  { 
  return GameActorBuilding::cast(getActor().get()); 
  }

const GameActorBuilding* BuildingComponent::getBuildingActor() const 
  {
  return GameActorBuilding::cast(getActor().get()); 
  }
  