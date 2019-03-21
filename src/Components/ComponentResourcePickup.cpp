//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentResourcePickup.h"
#include "GameActorTypes/GameActorResource.h"


/*
*   ComponentResourcePickupBlueprint
*/

bool ComponentResourcePickupBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  pickupPos = xmlGetVec3Value(xmlComponent, OD_PICKUP);
  return true;
  }

bool ComponentResourcePickupBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  return true;
  }

SMComponentPtr ComponentResourcePickupBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  ComponentResourcePickup* component = new ComponentResourcePickup(actor, type);
  component->pickupPos = pickupPos;
  return SMComponentPtr(component);
  }



/*
*   ComponentResourcePickup
*/

ComponentResourcePickup::ComponentResourcePickup(const SMGameActorPtr& actor, SMComponentType type) : SMComponent(actor, type)
  {}

void ComponentResourcePickup::initialise(GameContext* gameContext)
  {
  }

void ComponentResourcePickup::update(GameContext* gameContext)
  {
  if (updateTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    uint gotResourceID = 0;
    getActor()->forEachResource([&](uint id, uint amount)
      {
      gotResourceID = id;
      });

    if (gotResourceID == 0)
      {
      if (carryingResource)
        {
        if (auto resActor = resourceActor.lock())
          SMGameContext::cast(gameContext)->destroySMActor(resActor->getID());
        carryingResource = false;
        resourceActor.reset();
        }
      return;
      }

    bool createNewResActor = true;
    if (carryingResource)
      {
      if (auto resActor = resourceActor.lock())
        createNewResActor = resActor->getBlueprintTypeID() != gotResourceID;
      }
    carryingResource = true;
    if (createNewResActor)
      createResourceActor(gameContext, gotResourceID);

    updateTimer.setTimeOut(500);
    updateTimer.reset();
    }

  if (carryingResource)
    updateResourceActorTransform();
  }

void ComponentResourcePickup::cleanUp(GameContext* gameContext)
  {
  if (auto resActor = resourceActor.lock())
    SMGameContext::cast(gameContext)->destroySMActor(resActor->getID());
  carryingResource = false;
  resourceActor.reset();
  }


void ComponentResourcePickup::createResourceActor(GameContext* gameContext, uint resourceBlueprintID)
  {
  if (auto resActor = resourceActor.lock())
    SMGameContext::cast(gameContext)->destroySMActor(resActor->getID());
  SMGameActorPtr actor = SMGameContext::cast(gameContext)->createSMGameActor(resourceBlueprintID, Vector3D());
  actor->setAllowSaving(false);
  resourceActor = actor;
  }

void ComponentResourcePickup::updateResourceActorTransform()
  {
  SMGameActor* actor = getActor().get();
  SMGameActor* resActor = resourceActor.lock().get();
  if (!resActor)
    return;


  const Vector2D midPos = actor->getMidPosition();
  const double rotation = actor->getRotation();

  Transform transform;
  transform.translate(pickupPos);
  transform.rotate(0, 1, 0, rotation);
  transform.translate(Vector3D(midPos.x, 0, -midPos.y));
  resActor->setPosition(transform.transform(Vector3D(0, 0, 0)));
  resActor->setRotation(rotation);
  }