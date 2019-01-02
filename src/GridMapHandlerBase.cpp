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
        griddedActors[gridPosToIndex(pos)] = id;
      }
    }
  }

uint GridMapHandlerBase::getIDAtGridPos(const GridXY& gridPos) const
  {
  if (!isOnMap(gridPos))
    return 0;
  return griddedActors[gridPosToIndex(gridPos)];
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
  return griddedActors[gridPosToIndex(gridPos)] == 0;
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


SMStaticActorPtr GridMapHandlerBase::findClosestStaticActor(GameContext* gameContext, GridXY position, GridMapHandlerBase::FindActorPredicate predicate) const
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  //  dreadful brute force method for now
  SMStaticActorPtr closestActor;
  double closestDistance = DBL_MAX;
  for (auto actorID : griddedActors)
    {
    SMStaticActorPtr actor = smGameContext->getStaticActor(actorID);
    if (!actor || !predicate(actor))
      continue;

//    GridMapPath path;
//    if (!getPathToTarget(position, actor->getGridPosition(), &path))
//      continue;

    double thisDistance = (actor->getPosition() - position.centre()).magnitude();
    if (thisDistance < closestDistance)
      {
      closestDistance = thisDistance;
      closestActor = actor;
      }
    }

  return closestActor;
  }


SMStaticActorPtr GridMapHandlerBase::findClosestStaticActor(GameContext* gameContext, GridXY position, GameObjectType gameObjDefType) const
  {
  const auto gameObjectFactory = SMGameContext::cast(gameContext)->getGameObjectFactory();
  return findClosestStaticActor(gameContext, position, [&](SMStaticActorPtr actor)
    {
    auto type = actor->getDef()->getType();
    return gameObjectFactory->isTypeOrSubType(gameObjDefType, type);
    });
  }

SMStaticActorPtr GridMapHandlerBase::findClosestStaticActor(GameContext* gameContext, GridXY position, string gameObjDefName) const
  {
  return findClosestStaticActor(gameContext, position, [&](SMStaticActorPtr actor)
    {
    return actor->getDef()->getUniqueName() == gameObjDefName;
    });
  }

bool GridMapHandlerBase::getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const
  {
  if (subGetPathToTarget(startPos, targetPos, path, maxPathLength))
    {
    path->setMapVersion(mapVersion);
    return true;
    }
  path->setMapVersion(-1);
  return false;
  }

bool GridMapHandlerBase::isPathInvalid(GridMapPath* path) const
  {
  return path->getMapVersion() != mapVersion;
  }


bool GridMapSimpleDirect::subGetPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const
  {
  path->addNode(startPos);
  path->addNode(targetPos);
  return path->getLength() <= maxPathLength;
  }


bool GridMapSimpleManhattan::subGetPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const
  {
  path->addNode(startPos);
  path->addNode(GridXY(startPos.x, targetPos.y));
  path->addNode(targetPos);
  return path->getLength() <= maxPathLength;
  }
