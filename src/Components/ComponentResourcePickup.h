#pragma once

#include <Components/BuildingComponent.h>
#include <SMGameObjectFactory.h>

/*
*   
*/


class ComponentResourcePickupBlueprint : public SMComponentBlueprint
  {
private:
  Vector3D pickupPos;

public:
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class ComponentResourcePickup : public SMComponent
  {
private:
  friend class ComponentResourcePickupBlueprint;
  Vector3D pickupPos;
  bool carryingResource = false;
  SMGameActorWkPtr resourceActor;
  Timer updateTimer;

public:
  ComponentResourcePickup(const SMGameActorPtr& actor, SMComponentType type);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;

protected:
  void createResourceActor(GameContext* gameContext, uint resourceBlueprintID);
  void updateResourceActorTransform();
  };
