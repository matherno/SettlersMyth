//
// Created by matt on 27/02/19.
//

#include "GameActorUnit.h"
#include "BlueprintFileHelper.h"
#include "SMGameContext.h"


/*
*   GameActorUnitBlueprint
*/  

bool GameActorUnitBlueprint::loadFromXML(XMLElement* xmlElement, string* errorMsg)
  {
  if(!SMGameActorBlueprint::loadFromXML(xmlElement, errorMsg))
    return false;

  XMLElement* xmlUnit = xmlElement->FirstChildElement(OD_UNIT);
  if (xmlUnit)
    xmlUnit->QueryAttribute(OD_SPEED, &speed);

  return true;
  }
  
bool GameActorUnitBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  if(!SMGameActorBlueprint::finaliseLoading(gameContext, errorMsg))
    return false;

  return true;
  }

SMGameActor* GameActorUnitBlueprint::doConstructActor(uint actorID) const
  {
  GameActorUnit* unit = new GameActorUnit(actorID, id);
  unit->setupStackCount(1, 1);
  unit->speed = speed;
  return unit;
  }


/*
*   UnitCommand
*/  

void UnitCommand::startCommand(GameContext* gameContext, GameActorUnit* unit)
  {
  ASSERT(isCommandEnded(), "Command already started?");
  commandRunning = true;
  onStart(gameContext, unit);
  }

void UnitCommand::updateCommand(GameContext* gameContext, GameActorUnit* unit)
  {
  if (commandRunning)
    {
    onUpdate(gameContext, unit);
    if (gotTarget && commandRunning)
      moveToTarget(gameContext, unit);
    }
  }
  
void UnitCommand::cancelCommand(GameContext* gameContext, GameActorUnit* unit)
  {
  commandRunning = false;
  onEnd(gameContext, unit, true);
  GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
  if(attachedBuilding)
    attachedBuilding->postMessage(gameContext, SMMessage::attachedUnitCmdCancelled, this);
  }
  
void UnitCommand::endCommand(GameContext* gameContext, GameActorUnit* unit)
  {
  commandRunning = false;
  onEnd(gameContext, unit, false);
  GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
  if(attachedBuilding)
    attachedBuilding->postMessage(gameContext, SMMessage::attachedUnitCmdEnded, this);
  }
  
bool UnitCommand::isCommandEnded()
  {
  return !commandRunning;
  }
  
void UnitCommand::setTargetPosition(const Vector2D& position, double rotation)
  {
  pathToTarget.reset();
  gotTarget = true;
  targetPosition = position;
  targetRotation = rotation;
  }
  
void UnitCommand::setTargetPosition(GridXY position, double rotation)
  {
  setTargetPosition(position.centre(), rotation);
  }

void UnitCommand::clearTarget()
  {
  gotTarget = false;
  pathToTarget.reset();
  }


void UnitCommand::moveToTarget(GameContext* gameContext, GameActorUnit* unit)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  const GridMapHandler* gridMapHandler = smGameContext->getGridMapHandler();

  //  if path is now invalid (map has changed...) kill it
  if (pathToTarget && gridMapHandler->isPathInvalid(pathToTarget.get()))
    pathToTarget.reset();

  if (!pathToTarget)
    {
    //  we don't have a path, so try obtain one
    pathToTarget.reset(new GridMapPath());
    if (unit->getGridPosition() == GridXY(targetPosition))
      {
      pathToTarget->addNode(targetPosition);
      }
    else if (gridMapHandler->getPathToTarget(unit->getPosition(), targetPosition, pathToTarget.get()))
      {
      if (pathToTarget->nodeCount() > 1 && pathToTarget->getTopPathNode() == unit->getGridPosition())
        pathToTarget->popTopPathNode();   // if first node is the cell we are in, pop it off
      }
    else
      {
      pathToTarget = nullptr;
      }
    }

  if (!pathToTarget)
    {
    //  don't have a path at this point, so we musn't have been able to obtain one
    gotTarget = false;
    onCantReachTarget(gameContext, unit);
    return;
    }

  if (pathToTarget->nodeCount() > 0)
    {
    //  move towards target along the obtained path
    Vector2D nodePosition = pathToTarget->getTopPathNode().centre();
    if (pathToTarget->nodeCount() == 1)
      nodePosition = targetPosition;

    const double maxDistance = ((double) gameContext->getDeltaTime() / 1000.0) *  unit->getMovementSpeed();
    Vector2D posToNode = nodePosition - unit->getPosition();
    if (posToNode.magnitude() < maxDistance)
      {
      unit->setPosition(nodePosition);
      pathToTarget->popTopPathNode();
      }
    else
      {
      unit->setPosition(unit->getPosition() + posToNode.getUniform() * (float) maxDistance);
      }
    unit->setRotation(-1 * mathernogl::ccwAngleBetween(Vector2D(0, 1), posToNode.getUniform()));
    }

  if (pathToTarget->nodeCount() == 0)
    {
    //  must have reached target
    unit->setRotation(targetRotation);
    onReachedTarget(gameContext, unit);
    clearTarget();
    }
  }



/*
*   ReturnToBaseUnitCommand
*/  

ReturnToBaseUnitCommand::ReturnToBaseUnitCommand(ResourceReserve resourceToDropOff) : resourceToDropOff(resourceToDropOff)
  {
  
  }

ReturnToBaseUnitCommand::ReturnToBaseUnitCommand()
  {
  
  }

void ReturnToBaseUnitCommand::onStart(GameContext* gameContext, GameActorUnit* unit)
  {
  GameActorBuilding* building = unit->getAttachedBuilding(gameContext);
  if (building)
    setTargetPosition(building->getEntryPosition());
  else
    cancelCommand(gameContext, unit);
  }
  
void ReturnToBaseUnitCommand::onReachedTarget(GameContext* gameContext, GameActorUnit* unit)
  {
  GameActorBuilding* building = unit->getAttachedBuilding(gameContext);
  if (building && resourceToDropOff.isValid())
    {
    building->fillResourceSpace(resourceToDropOff);
    unit->clearAllResources();
    }
  endCommand(gameContext, unit);
  }



/*
*   CollectResourceUnitCommand
*/  

CollectResourceUnitCommand::CollectResourceUnitCommand(uint actorIDToCollectFrom, uint resourceID)
     : actorIDToCollectFrom(actorIDToCollectFrom), resourceID(resourceID)
  {
  
  }

void CollectResourceUnitCommand::onStart(GameContext* gameContext, GameActorUnit* unit)
  {
  attachedBuildingResReserve.reset();
  collectionResLock.reset();

  GameActorBuilding* building = unit->getAttachedBuilding(gameContext);
  if (!building)
    {
    cancelCommand(gameContext, unit);
    return;
    }

  SMGameActorPtr targetActor = SMGameContext::cast(gameContext)->getSMGameActor(actorIDToCollectFrom);
  if (!targetActor)
    {
    cancelCommand(gameContext, unit);
    return;
    }

  attachedBuildingResReserve = building->reserveResourceSpace(resourceID, 1);
  collectionResLock = targetActor->lockResource(resourceID, 1);
  if (!attachedBuildingResReserve.isValid() || !collectionResLock.isValid())
    {
    cancelCommand(gameContext, unit);
    return;
    }

  GameActorBuilding* targetBuilding = GameActorBuilding::cast(targetActor.get());
  if (targetBuilding)
    setTargetPosition(targetBuilding->getEntryPosition());
  else
    setTargetPosition(targetActor->getPosition());
  unit->dropAllResources(gameContext);
  }
  
void CollectResourceUnitCommand::onReachedTarget(GameContext* gameContext, GameActorUnit* unit)
  {
  SMGameActorPtr targetActor = SMGameContext::cast(gameContext)->getSMGameActor(actorIDToCollectFrom);  
  if (targetActor && collectionResLock.isValid())
    {
    targetActor->takeLockedResource(collectionResLock);
    unit->storeResource(collectionResLock.resID, collectionResLock.resAmount);
    }
  endCommand(gameContext, unit);
  }

void CollectResourceUnitCommand::onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled)
  {
  if (wasCancelled)
    {
    GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
    if (attachedBuilding && attachedBuildingResReserve.isValid())
      attachedBuilding->freeResourceSpace(attachedBuildingResReserve);

    SMGameActorPtr targetActor = SMGameContext::cast(gameContext)->getSMGameActor(actorIDToCollectFrom);    
    if (targetActor && collectionResLock.isValid())
      targetActor->unlockResource(collectionResLock);
    }

  unit->returnToAttachedBuilding(gameContext, attachedBuildingResReserve);
  }
  



/*
*   GameActorUnit
*/  

GameActorUnit::GameActorUnit(uint id, uint typeID) : SMGameActor(id, typeID)
  {
  }

void GameActorUnit::onAttached(GameContext* gameContext)
  {
  SMGameActor::onAttached(gameContext);
  }

void GameActorUnit::onUpdate(GameContext* gameContext)
  {
  SMGameActor::onUpdate(gameContext);

  if (currentCommand)
    {
    UnitCommandPtr runningCommand = currentCommand;
    if (runningCommand->isCommandEnded())
      runningCommand->startCommand(gameContext, this);
    else
      runningCommand->updateCommand(gameContext, this);
    if (runningCommand->isCommandEnded() && currentCommand == runningCommand)
      currentCommand.reset();
    }
  }

void GameActorUnit::onDetached(GameContext* gameContext)
  {
  if (currentCommand)
    currentCommand->cancelCommand(gameContext, this);
  currentCommand.reset();

  SMGameActor::onDetached(gameContext);
  }

void GameActorUnit::performCommand(UnitCommandPtr command)
  {
  ASSERT(isIdling(), "Trying to perform command whist another is in progress!");
  if (isIdling())
    currentCommand = command;
  }

void GameActorUnit::returnToAttachedBuilding(GameContext* gameContext, ResourceReserve resourceToDropOff)
  {
  if (currentCommand && !currentCommand->isCommandEnded())
    currentCommand->cancelCommand(gameContext, this);
  currentCommand.reset();
  performCommand(UnitCommandPtr(new ReturnToBaseUnitCommand(resourceToDropOff)));
  }


bool GameActorUnit::isIdling() const
  {
  if(currentCommand)
    return false;
  return true;
  }

GameActorBuilding* GameActorUnit::getAttachedBuilding(GameContext* gameContext) const
  {
  return GameActorBuilding::cast(SMGameContext::cast(gameContext)->getSMGameActor(attachedBuilding).get());
  }
