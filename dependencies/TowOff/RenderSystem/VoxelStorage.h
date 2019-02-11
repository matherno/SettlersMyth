#pragma once

#include <vector>
#include <mathernogl/MathernoGL.h>

/*
*   
*/

class VoxelStorage
  {
private:
  const uint id;
  mathernogl::VertexArray vao;
  uint numVoxels = 0;
  mathernogl::VoxelPoint min;
  mathernogl::VoxelPoint max;
  bool minMaxValid = false;

public:
  std::vector<mathernogl::Vector3D> colours;
  std::vector<mathernogl::VoxelPoint> voxels;

  VoxelStorage(uint id) : id(id) {}
  bool initialiseVAO();
  void cleanUp();
  mathernogl::VertexArray getVAO() { return vao; }
  uint getID() const { return id; }
  uint getNumVoxels() const { return numVoxels; }
  void getMin(int& x, int& y, int& z);
  void getMax(int& x, int& y, int& z);
  mathernogl::Vector3D getMin();
  mathernogl::Vector3D getMax();
  void invalidateMinMax();

protected:
  void recalculateMinMax();
  };
typedef std::shared_ptr<VoxelStorage> VoxelStoragePtr;

/*

class VoxelBatchStorage
  {
private:
  struct VoxelModelInstance
    {
    mathernogl::Vector3D transform;
    std::string filePath;
    };

  struct VoxelModelData
    {
    std::string filePath;
    mathernogl::Vector3D min;
    mathernogl::Vector3D max;
    std::vector<mathernogl::VoxelPoint> voxels;
    };

  std::map<std::string, VoxelModelData> voxelModels;
  std::map<uint, VoxelModelInstance> voxelModelInstances;

  mathernogl::VertexArray vao;
  mathernogl::GPUBufferStatic perInstanceVoxelBuffer;
  uint numVoxels = 0;
  double voxelSize = 1;
  std::vector<mathernogl::Vector3D> coloursArray;
  uint nextVoxelModelID = 1;

public:
  VoxelBatchStorage() {}
  void setVoxelSize(double voxelSize) { this->voxelSize = voxelSize; }
  bool initialiseVAO();
  void cleanUp();
  uint addVoxelModel(const std::string& filePath, const mathernogl::Vector3D& transform);
  void removeVoxelModel(uint voxelModelID);
  mathernogl::Vector3D getMin(uint voxelModelID);
  mathernogl::Vector3D getMax(uint voxelModelID);

  mathernogl::VertexArray getVAO() { return vao; }
  uint getNumVoxels() const { return numVoxels; }

protected:
  uint addColour(const mathernogl::Vector3D& colour);
  void calculateMinMax(VoxelModelData& model);
  void updateInstanceBuffer();
  };
typedef std::shared_ptr<VoxelBatchStorage> VoxelBatchStoragePtr;

*/