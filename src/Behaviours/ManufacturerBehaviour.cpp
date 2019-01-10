//
// Created by matt on 16/12/18.
//

#include <Building.h>
#include <GameObjectDefs/BuildingManufacturerDef.h>
#include <SMGameContext.h>
#include "ManufacturerBehaviour.h"


void ManufacturerBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {
  sendUnitTimer.setTimeOut(2000);
  sendUnitTimer.reset();

  manufactureCooldownTimer.setTimeOut(2500);
  manufactureCooldownTimer.reset();

  resCollectIdx = 0;
  resCollectCycle.clear();
  const BuildingManufacturerDef* manufDef = BuildingManufacturerDef::cast(gameActor->getDef());
  for (auto pair : manufDef->inputs)
    {
    const string resName = pair.first;
    const uint resID = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDefID(resName);
    if (resID > 0)
      {
      for (int resNum = 0; resNum < pair.second; ++resNum)
        resCollectCycle.push_back(resID);
      }
    }
  std::shuffle(resCollectCycle.begin(), resCollectCycle.end(), std::default_random_engine());
  }

void ManufacturerBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  Building* building = Building::cast(gameActor);
  if (!building)
    {
    ASSERT(false, "");
    return;
    }

  const BuildingManufacturerDef* manufDef = BuildingManufacturerDef::cast(building->getDef());
  if (!manufDef)
    {
    ASSERT(false, "");
    return;
    }


  if (!isManufacturing && manufactureCooldownTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    //  start manufacture process if can
    manufactureCooldownTimer.reset();
    if (canStartManufacturing(gameActor, gameContext))
      {
      UnitPtr manufactureUnit = building->getIdleUnit();
      if (manufactureUnit && manufactureUnit->processCommand(CMD_MOVE_TO_MANUF_SPOT, gameContext))
        {
        manufacturingUnitID = manufactureUnit->getID();
        startObservingItem(manufactureUnit);
        isManufacturing = true;
        manufactureCooldownTimer.setPaused(true);
        }
      }
    }

  //  send out unit to collect input resource
  if (sendUnitTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    uint resToCollect = getNextResourceToCollect(gameActor);
    UnitPtr unit = building->getIdleUnit();
    if (unit && resToCollect > 0 && building->getIdleUnitCount() > 1)
      unit->processCommand(SMActorCommand(CMD_COLLECT, resToCollect), gameContext);
    building->returnIdleUnits(gameActor, gameContext);
    sendUnitTimer.reset();
    }
  }

void ManufacturerBehaviour::observedCommandFinish(GameContext* gameContext, SMObserverItem* observed, uint cmdID)
  {
  Unit* unit = Unit::cast(observed);
  if (!unit)
    return;

  if (unit->getID() == manufacturingUnitID && isManufacturing)
    {
    if (cmdID == CMD_MANUFACTURE)
      {
      resetManufacturing();
      }
    else if (cmdID == CMD_MOVE_TO_MANUF_SPOT)
      {
      if (!unit->processCommand(CMD_MANUFACTURE, gameContext))
        resetManufacturing();
      }
    }
  }

void ManufacturerBehaviour::observedDestroyed(GameContext* gameContext, SMObserverItem* observed)
  {
  Unit* unit = Unit::cast(observed);
  if (!unit)
    return;

  if (unit->getID() == manufacturingUnitID)
    resetManufacturing();
  }

void ManufacturerBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {

  }

uint ManufacturerBehaviour::getNextResourceToCollect(SMGameActor* gameActor)
  {
  Building* building = Building::cast(gameActor);
  if (!building)
    {
    ASSERT(false, "");
    return 0;
    }

  uint resToCollect = 0;
  int numResChecked = 0;
  while (numResChecked < resCollectCycle.size())
    {
    resToCollect = resCollectCycle[resCollectIdx];
    resCollectIdx++;
    if (resCollectIdx >= resCollectCycle.size())
      resCollectIdx = 0;
    numResChecked++;
    if (gameActor->canStoreResource(resToCollect, 1))
      break;
    else
      resToCollect = 0;
    }

  return resToCollect;
  }

bool ManufacturerBehaviour::canStartManufacturing(SMGameActor* gameActor, GameContext* gameContext)
  {
  const BuildingManufacturerDef* manufDef = BuildingManufacturerDef::cast(gameActor->getDef());
  if (!manufDef)
    {
    ASSERT(false, "");
    return false;
    }

  const uint outputResID = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDefID(manufDef->outputResName);
  if(!gameActor->canStoreResource(outputResID, manufDef->outputResAmount))
    return false;

  for (auto pair : manufDef->inputs)
    {
    const uint resID = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameObjectDefID(pair.first);
    if (gameActor->resourceCount(resID) < pair.second)
      return false;
    }
  return true;
  }

void ManufacturerBehaviour::resetManufacturing()
  {
  manufactureCooldownTimer.setPaused(false);
  manufactureCooldownTimer.reset();
  isManufacturing = false;
  manufacturingUnitID = 0;
  }



