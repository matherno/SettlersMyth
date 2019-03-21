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
  int manhattanDistance(const GridXY& pt) const { return abs(pt.x-x) + abs(pt.y-y); }
  double distance(const GridXY& pt) const { return sqrt(pow(pt.x-x, 2) + pow(pt.y-y, 2)); }
  Vector2D centre() const { return Vector2D(0.5f + x, 0.5f + y); }
  Vector2D operator+ (const Vector2D& pt) const { return Vector2D(pt.x+x, pt.y+y); }
  GridXY operator+ (const GridXY& pt) const { return GridXY(pt.x+x, pt.y+y); }
  GridXY operator- (const GridXY& pt) const { return GridXY(pt.x+x, pt.y+y); }
  bool operator== (const GridXY& pt) const { return pt.x == x && pt.y == y; }
  bool operator!= (const GridXY& pt) const { return !(*this == pt); }
  operator Vector2D () const { return Vector2D(x, y); }
  operator Vector3D () const { return Vector3D(x, 0, -y); }
  };