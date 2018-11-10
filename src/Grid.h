#pragma once

#include <cstdlib>
#include <TowOff/RenderSystem/RenderSystem.h>

/*
*   
*/

struct GridXY
  {
  int x = 0;
  int y = 0;
  GridXY() {}
  GridXY(const Vector2D& vector) { x = floor(vector.x); y = floor(vector.y); }
  GridXY(const Vector3D& vector) { x = floor(vector.x); y = -1 * floor(vector.z); }
  GridXY(int x, int y) { this->x = x; this->y = y; }
  int manhattanDistance(const GridXY& pt) { return abs(pt.x-x) + abs(pt.y-y); }
  GridXY operator+ (const GridXY& pt) const { return GridXY(pt.x+x, pt.y+y); }
  GridXY operator- (const GridXY& pt) const { return GridXY(pt.x+x, pt.y+y); }
  operator Vector3D () { return Vector3D(x, 0, -y); }
  };