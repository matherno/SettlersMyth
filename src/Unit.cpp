//
// Created by matt on 1/12/18.
//

#include "Unit.h"
#include "SMGameContext.h"
#include "Building.h"


Unit::Unit(uint id, const IGameObjectDef* gameObjectDef) : SMDynamicActor(id, gameObjectDef)
  {
  }

void Unit::onUpdate(GameContext* gameContext)
  {
  if (gotTarget && !cantReachTarget && !hasReachedTarget())
    {
    SMGameContext* smGameContext = SMGameContext::cast(gameContext);
    const GridMapHandlerBase* gridMapHandler = smGameContext->getGridMapHandler();

    //  if path is now invalid (map has changed...) kill it
    if (pathToTarget && gridMapHandler->isPathInvalid(pathToTarget.get()))
      {
      pathToTarget.reset();
      }

    //  if we don't have a path, obtain one
    if (!pathToTarget)
      {
      pathToTarget.reset(new GridMapPath());
      if (gridMapHandler->getPathToTarget(getPosition(), targetPosition, pathToTarget.get()))
        {
        cantReachTarget = false;
        if (pathToTarget->nodeCount() > 1 && pathToTarget->getTopPathNode() == getPosition())
          pathToTarget->popTopPathNode();   // if first node is the cell we are in, pop it off
        }
      else
        {
        cantReachTarget = true;
        pathToTarget = nullptr;
        }
      }

    //  move towards target along the obtained path
    if (pathToTarget && pathToTarget->nodeCount() > 0)
      {
      Vector2D nodePosition = pathToTarget->getTopPathNode().centre();
      if (pathToTarget->nodeCount() == 1)
        nodePosition = targetPosition;

      const double maxDistance = ((double) gameContext->getDeltaTime() / 1000.0) * speed;
      Vector2D posToNode = nodePosition - getPosition();
      if (posToNode.magnitude() < maxDistance)
        {
        setPosition(nodePosition);
        pathToTarget->popTopPathNode();
        }
      else
        {
        setPosition(getPosition() + posToNode.getUniform() * (float) maxDistance);
        }
      setRotation(-1 * mathernogl::ccwAngleBetween(Vector2D(0, 1), posToNode.getUniform()));
      }
    }

  SMDynamicActor::onUpdate(gameContext);
  }

void Unit::onDetached(GameContext* gameContext)
  {
  if (attachedBuilding > 0)
    {
    auto building = Building::cast(SMGameContext::cast(gameContext)->getStaticActor(attachedBuilding).get());
    if (building)
      building->dettachUnit(getID());
    }
  SMGameActor::onDetached(gameContext);
  }

bool Unit::hasReachedTarget() const
  {
  if (!gotTarget)
    return false;
  return fabs(getPosition().x - targetPosition.x) < 1e-10 && fabs(getPosition().y - targetPosition.y) < 1e-10;
  }

bool Unit::canNotReachTarget() const
  {
  return gotTarget && cantReachTarget;
  }

void Unit::setTarget(Vector2D target)
  {
  gotTarget = true;
  targetPosition = target;
  pathToTarget.reset();
  }

void Unit::clearTarget()
  {
  gotTarget = false;
  cantReachTarget = false;
  }
