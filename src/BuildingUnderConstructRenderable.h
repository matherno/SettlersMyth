#pragma once

#include <RenderSystem/RenderableVoxels.h>

/*
*   
*/

class BuildingUnderConstructRenderable : public RenderableVoxels
  {
public:
  BuildingUnderConstructRenderable(uint id, int drawStage = DRAW_STAGE_OPAQUE);

  void setConstructionCompletion(float factorComplete);    //  0 -> 1 completion
  };
typedef std::shared_ptr<BuildingUnderConstructRenderable> BuildingUnderConstructRenderablePtr;
