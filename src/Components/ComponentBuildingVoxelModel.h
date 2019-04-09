#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>
#include "ComponentVoxelModel.h"

/*
*   Specialised building voxel model component that reacts to buildings under construction
*/


class ComponentBuildingVoxelModelBlueprint : public ComponentVoxelModelBlueprint
  {
public:
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };



class ComponentBuildingVoxelModel : public ComponentVoxelModel
  {
private:
  const ComponentBuildingVoxelModelBlueprint* blueprint;

public:
  ComponentBuildingVoxelModel(const SMGameActorPtr& actor, SMComponentType type, const ComponentBuildingVoxelModelBlueprint* blueprint);
  virtual void onMessage(GameContext* gameContext, SMMessage message, void* extra) override;

protected:
  virtual RenderablePtr createRenderable(GameContext* gameContext, const string& filePath) const override;
  };
