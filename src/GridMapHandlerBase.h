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


class GridMapHandlerBase
  {
protected:
  std::vector<uint> griddedActors;
  const GridXY mapSize;
  long mapVersion = 0;

public:
  GridMapHandlerBase(GridXY size);
  void setGridCells(uint id, const GridXY& gridPos, const GridXY& regionSize);
  uint getIDAtGridPos(const GridXY& gridPos) const;
  bool isOnMap(const Vector2D& gridPos) const;
  bool isOnMap(const GridXY& gridPos) const;
  bool isCellClear(const GridXY& gridPos) const;
  bool isRegionClear(const GridXY& gridPos, const GridXY& regionSize) const;
  GridXY getMapSize() const { return mapSize; }
  int gridPosToIndex(GridXY gridPos) const { return gridPos.x + gridPos.y * mapSize.x; }
  void incrementMapVersion() { ++mapVersion; }

  typedef std::function<bool(SMStaticActorPtr)> FindActorPredicate;
  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, FindActorPredicate predicate) const;
  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, GameObjectType gameObjDefType) const;
  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, string gameObjDefName) const;

  /*
   * Obtains the closest path from start position to target, adding the nodes to given path, including starting position
   */
  virtual bool getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength = DEFAULT_MAX_PATH) const;
  virtual bool isPathInvalid(GridMapPath* path) const;

protected:
  virtual bool subGetPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const = 0;
  };



class GridMapSimpleDirect : public GridMapHandlerBase
  {
public:
  GridMapSimpleDirect(const GridXY& size) : GridMapHandlerBase(size) {}

protected:
  virtual bool subGetPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const override;
  };


class GridMapSimpleManhattan : public GridMapHandlerBase
  {
public:
  GridMapSimpleManhattan(const GridXY& size) : GridMapHandlerBase(size) {}

protected:
  virtual bool subGetPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const override;
  };