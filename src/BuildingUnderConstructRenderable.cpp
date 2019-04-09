//
// Created by matt on 20/02/19.
//

#include "BuildingUnderConstructRenderable.h"

BuildingUnderConstructRenderable::BuildingUnderConstructRenderable(uint id, int drawStage)
  : RenderableVoxels(id, drawStage)
  {
  setBackFaceCulling(true);
  }

void BuildingUnderConstructRenderable::setConstructionCompletion(float factorComplete)
  {
  BoundingBoxPtr bbox = getBounds();
  int voxelsToShow = (int)floor((bbox->getUpperBound().y - bbox->getLowerBound().y) * factorComplete / getVoxelSize());
  if (voxelsToShow == 0)
    voxelsToShow = 1;
  double yClip = (getVoxelSize() * voxelsToShow) + bbox->getLowerBound().y;
  setClippingPlane(Vector4D(0, -1, 0, yClip));
  }
