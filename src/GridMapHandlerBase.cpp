//
// Created by matt on 5/12/18.
//

#include <cfloat>
#include "GridMapHandlerBase.h"
#include "SMGameContext.h"



void GridMapPath::addNode(GridXY node)
  {
  pathNodes.push_back(node);
  }

void GridMapPath::clearNodes()
  {
  pathNodes.clear();
  }

double GridMapPath::getLength() const
  {
  if (pathNodes.size() > 1)
    return getLength(pathNodes.front());
  return 0;
  }

double GridMapPath::getLength(GridXY startPos) const
  {
  if (pathNodes.empty())
    return 0;

  double distance = 0;
  GridXY prevNode = startPos;
  for (const auto& node : pathNodes)
    {
    distance += node.distance(prevNode);
    prevNode = node;
    }
  return distance;
  }

GridXY GridMapPath::getTopPathNode() const
  {
  if (nodeCount() > 0)
    return pathNodes.front();
  }

void GridMapPath::popTopPathNode()
  {
  if (nodeCount() > 0)
    pathNodes.pop_front();
  }

int GridMapPath::nodeCount() const
  {
  return (int)pathNodes.size();
  }





GridMapHandlerBase::GridMapHandlerBase(GridXY size) : mapSize(size)
  {
  griddedActors.clear();
  for (int x = 0; x < mapSize.x; ++x)
    {
    for (int y = 0; y < mapSize.y; ++y)
      griddedActors.push_back(0);
    }
  }

void GridMapHandlerBase::setGridCells(uint id, const GridXY& gridPos, const GridXY& regionSize)
  {
  for (int x = 0; x < regionSize.x; ++x)
    {
    for (int y = 0; y < regionSize.y; ++y)
      {
      GridXY pos = gridPos + GridXY(x, y);
      if (isOnMap(pos))
        griddedActors[pos.x + pos.y * mapSize.x] = id;
      }
    }
  }

uint GridMapHandlerBase::getIDAtGridPos(const GridXY& gridPos) const
  {
  if (!isOnMap(gridPos))
    return 0;
  return griddedActors[gridPos.x + gridPos.y * mapSize.x];
  }

bool GridMapHandlerBase::isOnMap(const Vector2D& gridPos) const
  {
  return isOnMap(GridXY(gridPos));
  }

bool GridMapHandlerBase::isOnMap(const GridXY& gridPos) const
  {
  return gridPos.x >= 0 && gridPos.x < mapSize.x && gridPos.y >= 0 && gridPos.y < mapSize.y;
  }

bool GridMapHandlerBase::isCellClear(const GridXY& gridPos) const
  {
  if (!isOnMap(gridPos))
    return false;
  return griddedActors[gridPos.x + gridPos.y * mapSize.x] == 0;
  }

bool GridMapHandlerBase::isRegionClear(const GridXY& gridPos, const GridXY& regionSize) const
  {
  for (int x = 0; x < regionSize.x; ++x)
    {
    for (int y = 0; y < regionSize.y; ++y)
      {
      if (!isCellClear(gridPos + GridXY(x, y)))
        return false;
      }
    }
  return true;
  }

SMStaticActorPtr GridMapHandlerBase::findClosestStaticActor(GameContext* gameContext, GridXY position, GameObjectType gameObjDefType) const
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  const auto gameObjectFactory = smGameContext->getGameObjectFactory();

  //  dreadful brute force method for noe
  SMStaticActorPtr closestActor;
  double closestDistance = DBL_MAX;
  for (auto actorID : griddedActors)
    {
    SMStaticActorPtr actor = smGameContext->getStaticActor(actorID);
    if (!actor)
      continue;

    auto type = actor->getDef()->getType();
    if(!gameObjectFactory->isTypeOrSubType(gameObjDefType, type))
      continue;

    GridMapPath path;
    if (!getPathToTarget(position, actor->getGridPosition(), &path))
      continue;

    double thisDistance = path.getLength();
    if (thisDistance < closestDistance)
      {
      closestDistance = thisDistance;
      closestActor = actor;
      }
    }

  return closestActor;
  }

SMStaticActorPtr GridMapHandlerBase::findClosestStaticActor(GameContext* gameContext, GridXY position, string gameObjDefName) const
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  const auto gameObjectFactory = smGameContext->getGameObjectFactory();

  //  dreadful brute force method for noe
  SMStaticActorPtr closestActor;
  double closestDistance = DBL_MAX;
  for (auto actorID : griddedActors)
    {
    SMStaticActorPtr actor = smGameContext->getStaticActor(actorID);
    if (!actor)
      continue;

    if(actor->getDef()->getUniqueName() != gameObjDefName)
      continue;

    GridMapPath path;
    if (!getPathToTarget(position, actor->getGridPosition(), &path))
      continue;

    double thisDistance = path.getLength();
    if (thisDistance < closestDistance)
      {
      closestDistance = thisDistance;
      closestActor = actor;
      }
    }

  return closestActor;
  }


bool GridMapSimpleDirect::getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path) const
  {
  path->addNode(startPos);
  path->addNode(targetPos);
  return true;
  }


bool GridMapSimpleManhattan::getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path) const
  {
  path->addNode(startPos);
  path->addNode(GridXY(startPos.x, targetPos.y));
  path->addNode(targetPos);
  return true;
  }
