#pragma once

#include <mathernogl/Types.h>
#include "Grid.h"
#include "SMGameActor.h"

/*
*
*/

class GridMapPath
  {
private:
  std::list<GridXY> pathNodes;

public:
  GridXY getTopPathNode() const;
  void popTopPathNode();
  int nodeCount() const;
  void addNode(GridXY node);
  void clearNodes();
  double getLength() const;
  double getLength(GridXY startPos) const;
  };


class GridMapHandlerBase
  {
protected:
  std::vector<uint> griddedActors;
  const GridXY mapSize;

public:
  GridMapHandlerBase(GridXY size);
  void setGridCells(uint id, const GridXY& gridPos, const GridXY& regionSize);
  uint getIDAtGridPos(const GridXY& gridPos) const;
  bool isOnMap(const Vector2D& gridPos) const;
  bool isOnMap(const GridXY& gridPos) const;
  bool isCellClear(const GridXY& gridPos) const;
  bool isRegionClear(const GridXY& gridPos, const GridXY& regionSize) const;
  GridXY getMapSize() const { return mapSize; }


  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, GameObjectType gameObjDefType) const;
  SMStaticActorPtr findClosestStaticActor(GameContext* gameContext, GridXY position, string gameObjDefName) const;

  /*
   * Obtains the closest path from start position to target, adding the nodes to given path, including starting position
   */
  virtual bool getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path) const = 0;
  };



class GridMapSimpleDirect : public GridMapHandlerBase
  {
public:
  GridMapSimpleDirect(const GridXY& size) : GridMapHandlerBase(size) {}

  virtual bool getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path) const override;
  };


class GridMapSimpleManhattan : public GridMapHandlerBase
  {
public:
  GridMapSimpleManhattan(const GridXY& size) : GridMapHandlerBase(size) {}

  virtual bool getPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path) const override;
  };