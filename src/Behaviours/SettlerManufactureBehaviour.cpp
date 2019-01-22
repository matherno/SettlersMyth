//
// Created by matt on 17/12/18.
//

#include <SMGameContext.h>
#include <GameObjectDefs/BuildingManufacturerDef.h>
#include "SettlerManufactureBehaviour.h"


void SettlerManufactureBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  if (!isBehaviourActive())
    return;

  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return;

  auto manufDef = BuildingManufacturerDef::cast(buildingActor->getDef());
  if (!manufDef)
    return;

  Unit* unit = Unit::cast(gameActor);
  if (timer.incrementTimer(gameContext->getDeltaTime()))
    {
    const uint resID = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDefID(manufDef->outputResName);
    unit->clearAllResources();
    unit->storeResource(resID, manufDef->outputResAmount);
    timer.reset();
    endBehaviour(gameActor, gameContext, true);
    }
  }


bool SettlerManufactureBehaviour::processCommand(SMGameActor* gameActor, GameContext* gameContext, const SMActorCommand& command)
  {
  if (SettlerBehaviourBase::processCommand(gameActor, gameContext, command))
    return true;

  if (command.id != CMD_MANUFACTURE)
    return false;

  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  auto gameObjectFactory = smGameContext->getGameObjectFactory();
  auto buildingActor = getAttachedBuilding(gameActor, gameContext);
  if (!buildingActor)
    return false;

  auto manufDef = BuildingManufacturerDef::cast(buildingActor->getDef());
  if (!manufDef)
    return false;

  Unit* unit = Unit::cast(gameActor);
  unit->dropAllResources(gameContext, unit->getPosition());

  //  check we got all the required resources
  bool gotAllResources = true;
  for (auto pair : manufDef->inputs)
    {
    const uint resID = gameObjectFactory->findGameObjectDefID(pair.first);
    if (buildingActor->resourceCount(resID, false, true) < pair.second)
      gotAllResources = false;
    }
  if (!gotAllResources)
    return false;

  //  take all the required resources
  for (auto pair : manufDef->inputs)
    {
    const uint resID = gameObjectFactory->findGameObjectDefID(pair.first);
    buildingActor->takeResource(resID, pair.second, true);
    }

  //  start manufacture timer
  timer.setTimeOut(manufDef->manufactureTime);
  timer.reset();
  startBehaviour(gameActor, gameContext, command.id);
  return true;
  }

