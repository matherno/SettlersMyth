//
// Created by matt on 2/01/19.
//

#include "GridMapAStar.h"

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

bool GridMapAStar::subGetPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const
  {
  for (GridXY pos : std::vector<GridXY>{
        GridXY(startPos.x, startPos.y),
        GridXY(startPos.x, startPos.y+1),
        GridXY(startPos.x+1, startPos.y),
        GridXY(startPos.x, startPos.y-1),
        GridXY(startPos.x-1, startPos.y),
      })
    {
    if (isCellClear(pos) && getPathToTargetAStar(pos, targetPos, path, maxPathLength))
      return true;
    }
  return false;
  }


bool GridMapAStar::getPathToTargetAStar(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const
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
