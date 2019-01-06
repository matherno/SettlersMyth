//
// Created by matt on 5/12/18.
//

#include <cfloat>
#include "GridMapHandler.h"
#include "SMGameContext.h"
#include "Building.h"


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





GridMapHandler::GridMapHandler(GridXY size) : mapSize(size)
  {
  griddedActors.clear();
  for (int x = 0; x < mapSize.x; ++x)
    {
    for (int y = 0; y < mapSize.y; ++y)
      griddedActors.emplace_back();
    }
  }

void GridMapHandler::setGridCells(uint id, const GridXY& gridPos, const GridXY& regionSize)
  {
  for (int x = 0; x < regionSize.x; ++x)
    {
    for (int y = 0; y < regionSize.y; ++y)
      {
      GridXY pos = gridPos + GridXY(x, y);
      if (isOnMap(pos))
        griddedActors[gridPosToIndex(pos)].actorID = id;
      }
    }
  mapVersion++;
  updateCellConnectionIDs();
  }

uint GridMapHandler::getIDAtGridPos(const GridXY& gridPos) const
  {
  if (!isOnMap(gridPos))
    return 0;
  return griddedActors[gridPosToIndex(gridPos)].actorID;
  }

uint GridMapHandler::getConnectionIDAtGridPos(const GridXY& gridPos) const
  {
  if (!isOnMap(gridPos))
    return 0;
  return griddedActors[gridPosToIndex(gridPos)].connectionID;
  }

bool GridMapHandler::isOnMap(const Vector2D& gridPos) const
  {
  return isOnMap(GridXY(gridPos));
  }

bool GridMapHandler::isOnMap(const GridXY& gridPos) const
  {
  return gridPos.x >= 0 && gridPos.x < mapSize.x && gridPos.y >= 0 && gridPos.y < mapSize.y;
  }

bool GridMapHandler::isCellClear(const GridXY& gridPos) const
  {
  if (!isOnMap(gridPos))
    return false;
  return griddedActors[gridPosToIndex(gridPos)].actorID == 0;
  }

bool GridMapHandler::isRegionClear(const GridXY& gridPos, const GridXY& regionSize) const
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


SMStaticActorPtr GridMapHandler::findClosestStaticActor(GameContext* gameContext, GridXY position, GridMapHandler::FindActorPredicate predicate) const
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  //  if position is not clear, then we need to check adjacent cells instead
  std::list<GridXY> possibleStartPositions;
  if (isCellClear(position))
    {
    possibleStartPositions.push_back(position);
    }
  else
    {
    for (GridXY pos : std::vector<GridXY>{
          GridXY(position.x, position.y+1),
          GridXY(position.x+1, position.y),
          GridXY(position.x, position.y-1),
          GridXY(position.x-1, position.y),
        })
      {
      if (isCellClear(pos))
        possibleStartPositions.push_back(pos);
      }
    }

  SMStaticActorPtr closestActor;
  double closestDistance = DBL_MAX;
  for (auto gridCell : griddedActors)
    {
    //  check the target actor is valid and what we are looking for in predicate function
    SMStaticActorPtr actor = smGameContext->getStaticActor(gridCell.actorID);
    if (!actor || !predicate(actor))
      continue;

    //  check all possible start positions against the target actor position
    for (GridXY pos : possibleStartPositions)
      {
      double thisDistance = (actor->getEntryPosition().centre() - pos.centre()).magnitude();
      if (thisDistance < closestDistance && canReachTarget(pos, actor->getEntryPosition()))
        {
        closestDistance = thisDistance;
        closestActor = actor;
        }
      }
    }

  return closestActor;
  }


SMStaticActorPtr GridMapHandler::findClosestStaticActor(GameContext* gameContext, GridXY position, GameObjectType gameObjDefType) const
  {
  const auto gameObjectFactory = SMGameContext::cast(gameContext)->getGameObjectFactory();
  return findClosestStaticActor(gameContext, position, [&](SMStaticActorPtr actor)
    {
    auto type = actor->getDef()->getType();
    return gameObjectFactory->isTypeOrSubType(gameObjDefType, type);
    });
  }

SMStaticActorPtr GridMapHandler::findClosestStaticActor(GameContext* gameContext, GridXY position, string gameObjDefName) const
  {
  return findClosestStaticActor(gameContext, position, [&](SMStaticActorPtr actor)
    {
    return actor->getDef()->getUniqueName() == gameObjDefName;
    });
  }

bool GridMapHandler::getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const
  {
  path->setMapVersion(-1);
  if (isCellClear(startPos))
    {
    if(canReachTarget(startPos, targetPos) && getPathToTargetAStar(startPos, targetPos, path, maxPathLength))
      {
      path->setMapVersion(mapVersion);
      return true;
      }
    return false;
    }

  for (GridXY pos : std::vector<GridXY>{
        GridXY(startPos.x, startPos.y+1),
        GridXY(startPos.x+1, startPos.y),
        GridXY(startPos.x, startPos.y-1),
        GridXY(startPos.x-1, startPos.y),
      })
    {
    if (isCellClear(pos) && canReachTarget(pos, targetPos) && getPathToTargetAStar(pos, targetPos, path, maxPathLength))
      {
      path->setMapVersion(mapVersion);
      return true;
      }
    }

  return false;
  }

bool GridMapHandler::isPathInvalid(GridMapPath* path) const
  {
  return path->getMapVersion() != mapVersion;
  }

bool GridMapHandler::canReachTarget(GridXY startPos, GridXY targetPos) const
  {
  const uint startConnectionID = getConnectionIDAtGridPos(startPos);
  if (startConnectionID == 0)
    return false;

  if (isCellClear(targetPos))
    return startConnectionID == getConnectionIDAtGridPos(targetPos);

  //  can still reach target if it is not a clear cell
  //  just need to check adjacent cells
  for (int xOffset = -1; xOffset <= 1; ++xOffset)
    {
    for (int yOffset = -1; yOffset <= 1; ++yOffset)
      {
      const GridXY pos = targetPos + GridXY(xOffset, yOffset);
      if (startConnectionID == getConnectionIDAtGridPos(pos))
        return true;
      }
    }

  return false;
  }

void GridMapHandler::updateCellConnectionIDs()
  {
  for (auto& gridCell : griddedActors)
    gridCell.connectionID = 0;

  uint nextConnectionID = 1;
  uint cellIndex = 0;
  for (auto& gridCell : griddedActors)
    {
    if(gridCell.actorID == 0 && gridCell.connectionID == 0)
      recurseSetCellConnectionID(indexToGridPos(cellIndex), nextConnectionID++);
    cellIndex++;
    }

#ifndef NDEBUG
#if 0
  string debugString;
  for (int y = 0; y < mapSize.y; ++y)
    {
    for (int x = 0; x < mapSize.x; ++x)
      {
      debugString += std::to_string(getConnectionIDAtGridPos(GridXY(x, y)));
      }
    debugString += "\n";
    }
  mathernogl::logInfo(debugString);
#endif
#endif
  }

void GridMapHandler::recurseSetCellConnectionID(GridXY pos, uint connectionID)
  {
  griddedActors[gridPosToIndex(pos)].connectionID = connectionID;

  for (int xOffset = -1; xOffset <= 1; ++xOffset)
    {
    for (int yOffset = -1; yOffset <= 1; ++yOffset)
      {
      if (xOffset == 0 && yOffset == 0)
        continue;
      const GridXY neighbourPos = pos + GridXY(xOffset, yOffset);
      if (getConnectionIDAtGridPos(neighbourPos) == 0 && isCellClear(neighbourPos))
        recurseSetCellConnectionID(neighbourPos, connectionID);
      }
    }
  }



/*
 * A Star path finding implementation
 */

struct AStarNode;
typedef mathernogl::MappedList<AStarNode> AStarNodeList;

struct AStarNode
  {
  GridXY pos;
  double hCost = 0;
  double gCost = 0;
  double fCost = 0;
  int parentIndex = -1;
  };

static void createPathFromClosedNodes(int finalNodeIdx, AStarNodeList& closedNodesList, GridMapPath* path)
  {
  path->clearNodes();

  std::list<uint> indexPath;
  AStarNode* node = closedNodesList.getPtr(finalNodeIdx);
  indexPath.emplace_front(finalNodeIdx);
  while (node)
    {
    int nextNodeIdx = node->parentIndex;
    node = nullptr;
    if (nextNodeIdx >= 0 && closedNodesList.contains(nextNodeIdx))
      {
      indexPath.emplace_front(nextNodeIdx);
      node = closedNodesList.getPtr(nextNodeIdx);
      }
    }

  for (uint index : indexPath)
    path->addNode(closedNodesList.get(index).pos);
  }

bool GridMapHandler::getPathToTargetAStar(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const
  {
  auto gCostFunc = [&](const GridXY& pos)
    {
    return pos.distance(targetPos);
    };
  auto hCostFunc = [&](const GridXY& pos, const GridXY& parentPos)
    {
    return pos.distance(parentPos);
    };
  auto fCostFunc = [&](double gCost, double hCost)
    {
    return gCost + hCost;
    };

  AStarNodeList openNodeList;
  AStarNodeList closedNodeList;

  //  put the initial grid position node onto closed list
  int currNodeIdx = gridPosToIndex(startPos);
  closedNodeList.add(AStarNode(), currNodeIdx);
  AStarNode* currClosedNode = closedNodeList.getPtr(currNodeIdx);
  currClosedNode->gCost = gCostFunc(startPos);
  currClosedNode->hCost = hCostFunc(startPos, startPos);
  currClosedNode->fCost = fCostFunc(currClosedNode->gCost, currClosedNode->hCost);
  currClosedNode->pos = startPos;

  while (true)
    {

    //  explore adjacent nodes to current closed one
    for (int xOffset = -1; xOffset <= 1; ++xOffset)
      {
      for (int yOffset = -1; yOffset <= 1; ++yOffset)
        {
        if (xOffset == 0 && yOffset == 0)
          continue;
        const GridXY pos = currClosedNode->pos + GridXY(xOffset, yOffset);
        if (pos == targetPos)
          {
          //  reached the target, so create path and return true
          createPathFromClosedNodes(currNodeIdx, closedNodeList, path);
          path->addNode(targetPos);
          return true;
          }

        //  check that this node is a valid clear map position, and it hasn't been closed
        const int exploreNodeIdx = gridPosToIndex(pos);
        if (exploreNodeIdx < 0 || !isCellClear(pos) || closedNodeList.contains(exploreNodeIdx))
          continue;

        const double exploreHCost = currClosedNode->hCost + hCostFunc(pos, currClosedNode->pos);
        if (openNodeList.contains(exploreNodeIdx))
          {
          //  update open node if the new h cost is better then recorded one
          AStarNode* exploreNode = openNodeList.getPtr(exploreNodeIdx);
          if (exploreHCost < exploreNode->hCost)
            {
            exploreNode->parentIndex = currNodeIdx;
            exploreNode->hCost = exploreHCost;
            exploreNode->fCost = fCostFunc(exploreNode->gCost, exploreNode->hCost);
            }
          }
        else
          {
          //  create new open node and apply cost values
          openNodeList.add(AStarNode(), exploreNodeIdx);
          AStarNode* exploreNode = openNodeList.getPtr(exploreNodeIdx);
          exploreNode->gCost = gCostFunc(pos);
          exploreNode->hCost = exploreHCost;
          exploreNode->fCost = fCostFunc(exploreNode->gCost, exploreNode->hCost);
          exploreNode->parentIndex = currNodeIdx;
          exploreNode->pos = pos;
          }
        }
      }

    //  check that we have open nodes, if not then the target isn't reachable
    if (openNodeList.count() == 0)
      return false;

    //  find open node with lowest f cost
    double minFCost = maxPathLength;
    currClosedNode = nullptr;
    for (AStarNode& node : *openNodeList.getList())
      {
      if (node.fCost < minFCost || (node.fCost == minFCost && currClosedNode && node.gCost < currClosedNode->gCost))
        {
        minFCost = node.fCost;
        currClosedNode = &node;
        }
      }

    //  no reachable open nodes
    if (!currClosedNode)
      return false;

    //  close this open node
    currNodeIdx = gridPosToIndex(currClosedNode->pos);
    openNodeList.remove(currNodeIdx);
    closedNodeList.add(*currClosedNode, currNodeIdx);
    currClosedNode = closedNodeList.getPtr(currNodeIdx);
    }

  return false;
  }

