#pragma once

#include <vector>
#include <RaySplayConst.h>

/*
*   
*/

class VoxelStorage
  {
private:
  const uint id;
  mathernogl::VertexArray vao;
  uint numVoxels = 0;

public:
  std::vector<Vector3D> colours;
  std::vector<mathernogl::VoxelPoint> voxels;

  VoxelStorage(uint id) : id(id) {}
  bool initialiseVAO();
  void cleanUp();
  mathernogl::VertexArray getVAO() { return vao; }
  uint getID() const { return id; }
  uint getNumVoxels() const { return numVoxels; }
  };
typedef std::shared_ptr<VoxelStorage> VoxelStoragePtr;
