//
// Created by matt on 13/12/18.
//

#include <SMGameContext.h>
#include "CarryResourceBehaviour.h"
#include "GameObjectDefs/UnitDef.h"

void CarryResourceBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

void CarryResourceBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (updateTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    uint gotResourceID = 0;
    gameActor->forEachResource([&](uint id, uint amount)
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
        createNewResActor = resActor->getDef()->getID() != gotResourceID;
      }
    carryingResource = true;
    if (createNewResActor)
      createResourceActor(gameActor, gameContext, gotResourceID);

    updateTimer.setTimeOut(500);
    updateTimer.reset();
    }

  if (carryingResource)
    updateResourceActorTransform(gameActor);
  }

void CarryResourceBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (auto resActor = resourceActor.lock())
    SMGameContext::cast(gameContext)->destroySMActor(resActor->getID());
  carryingResource = false;
  resourceActor.reset();
  }


void CarryResourceBehaviour::createResourceActor(SMGameActor* gameActor, GameContext* gameContext, uint gameObjDefID)
  {
  if (auto resActor = resourceActor.lock())
    SMGameContext::cast(gameContext)->destroySMActor(resActor->getID());
  auto actor = SMGameContext::cast(gameContext)->createSMGameActor(gameObjDefID, Vector3D());
  actor->setAllowSaving(false);
  resourceActor = actor;
  }

void CarryResourceBehaviour::updateResourceActorTransform(SMGameActor* gameActor)
  {
  Vector3D pickupPos(0, 0.5, 0.2);
  if (auto unitDef = UnitDef::cast(gameActor->getDef()))
    pickupPos = unitDef->pickupPos;
  const Vector2D midPos = gameActor->getMidPosition();
  const double rotation = gameActor->getRotation();

  if(auto dynamicResActor = SMDynamicActor::cast(resourceActor.lock().get()))
    {
    Transform transform;
    transform.translate(pickupPos);
    transform.rotate(0, 1, 0, rotation);
    transform.translate(Vector3D(midPos.x, 0, -midPos.y));
    dynamicResActor->setPosition(transform.transform(Vector3D(0, 0, 0)));
    dynamicResActor->setRotation(rotation);
    }
  }
