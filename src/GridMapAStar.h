#pragma once

#include "GridMapHandlerBase.h"

/*
*   
*/

class GridMapAStar : public GridMapHandlerBase
  {
public:
  GridMapAStar(const GridXY& size) : GridMapHandlerBase(size) {}

protected:
  virtual bool subGetPathToTarget(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const override;
  virtual bool getPathToTargetAStar(GridXY startPos, GridXY targetPos, GridMapPath* path, double maxPathLength) const;
  };
