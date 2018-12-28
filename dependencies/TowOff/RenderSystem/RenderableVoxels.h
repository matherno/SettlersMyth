#pragma once

#include "RenderSystem.h"
#include "VoxelStorage.h"

/*
*   
*/

class RenderableVoxels : public Renderable
  {
private:
  VoxelStoragePtr storage;
  ShaderProgramPtr shaderProgram;
  double voxelSize = 1;

public:
  RenderableVoxels(uint id, int drawStage = DRAW_STAGE_OPAQUE);
  virtual void initialise(RenderContext* renderContext) override;
  virtual void cleanUp(RenderContext* renderContext) override;
  virtual void render(RenderContext* renderContext) override;

  void setVoxelStorage(VoxelStoragePtr storage);
  void setVoxelSize(double size);
  };
