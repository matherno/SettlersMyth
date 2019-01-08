#pragma once

#include <mathernogl/Types.h>
#include "Grid.h"
#include "SMGameActor.h"

#define DEFAULT_MAX_PATH 50

/*
*
*/

class GridMapPath
  {
private:
  std::list<GridXY> pathNodes;
  long mapVersion = -1;
  GridXY startPos;
  GridXY targetPos;
  double maxPathLength;

public:
  GridXY getTopPathNode() const;
  void popTopPathNode();
  int nodeCount() const;
  void addNode(GridXY node);
  void clearNodes();
  double getLength() const;
  double getLength(GridXY startPos) const;
  void setMapVersion(long version) { mapVersion = version; }
  long getMapVersion() const { return mapVersion; }
  };


class GridMapHandler
  {
protected:
  struct GridMapCell
    {
    uint actorID = 0;
    uint connectionID = 0;
    bool isObstacle = false;
    };

  std::vector<GridMapCell> griddedActors;
  const GridXY mapSize;
  long mapVersion = 0;
  bool performingGridTransaction = false;

public:
  GridMapHandler(GridXY size);
  void startGridTransaction();
  void endGridTransaction();
  void setGridCells(const GridXY& gridPos, const GridXY& regionSize, uint actorID, bool isObstacle);
  void setGridCellIsObstacle(const GridXY& gridPos, bool isObstacle);
  uint getIDAtGridPos(const GridXY& gridPos) const;
  uint getConnectionIDAtGridPos(const GridXY& gridPos) const;
  bool isOnMap(const Vector2D& gridPos) const;
  bool isOnMap(const GridXY& gridPos) const;
  bool isCellClear(const GridXY& gridPos) const;
  bool isCellObstacle(const GridXY& gridPos) const;
  bool isRegionClear(const GridXY& gridPos, const GridXY& regionSize) const;
  GridXY getMapSize() const { return mapSize; }
  int gridPosToIndex(GridXY gridPos) const { return gridPos.x + gridPos.y * mapSize.x; }
  GridXY indexToGridPos(uint index) const { return GridXY(index % mapSize.x, index / mapSize.y); }

  typedef std::function<bool(SMStaticActorPtr)> FindActorPredicate;
  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, FindActorPredicate predicate) const;
  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, GameObjectType gameObjDefType) const;
  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, string gameObjDefName) const;

  /*
   *  Obtains the closest path from start position to target, adding the nodes to given path, including starting position
   */
  bool getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength = DEFAULT_MAX_PATH) const;
  bool isPathInvalid(GridMapPath* path) const;

  /*
   *  Determines if a path can be traced from start to target
   */
  bool canReachTarget(GridXY startPos, GridXY targetPos) const;

protected:
  void updateCellConnectionIDs();
  void recurseSetCellConnectionID(GridXY pos, uint connectionID);
  bool getPathToTargetAStar(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const;
  };

