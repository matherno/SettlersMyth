//
// Created by matt on 25/02/19.
//

#include "ComponentBuildingVoxelModel.h"
#include "GameActorTypes/GameActorBuilding.h"
#include "BuildingUnderConstructRenderable.h"

/*
*   ComponentBuildingVoxelModelBlueprint
*/


SMComponentPtr ComponentBuildingVoxelModelBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  if (!modelFilePaths.empty())
    return SMComponentPtr(new ComponentBuildingVoxelModel(actor, type, this));
  return nullptr;
  }





/*
*   ComponentBuildingVoxelModel
*/

ComponentBuildingVoxelModel::ComponentBuildingVoxelModel(const SMGameActorPtr& actor, SMComponentType type, const ComponentBuildingVoxelModelBlueprint* blueprint)
    : ComponentVoxelModel(actor, type, blueprint), blueprint(blueprint)
  {
  }

void ComponentBuildingVoxelModel::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  ComponentVoxelModel::onMessage(gameContext, message, extra);

  switch (message)
    {
    case SMMessage::constructionProgress:
      {
      GameActorBuilding* building = GameActorBuilding::cast(getActor().get());
      BuildingUnderConstructRenderable* underConstrRenderable = dynamic_cast<BuildingUnderConstructRenderable*>(voxelModel.get());
      if (building && underConstrRenderable)
        underConstrRenderable->setConstructionCompletion(building->getConstructionProgress());
      break;
      }
    case SMMessage::constructionFinished:
      refreshRenderable(gameContext);
      break;
    }
  }

RenderablePtr ComponentBuildingVoxelModel::createRenderable(GameContext* gameContext, const string& filePath) const
  {
  GameActorBuilding* building = GameActorBuilding::cast(getActor().get());
  if (building && building->getIsUnderConstruction())
    {
    RenderContext* renderContext = gameContext->getRenderContext();
    BuildingUnderConstructRenderable* voxels = new BuildingUnderConstructRenderable(renderContext->getNextRenderableID());
    voxels->setVoxelStorage(renderContext->getSharedVoxelStorage(filePath));
    voxels->setVoxelSize(blueprint->staticModel ? VOXEL_SIZE : DYNAMIC_VOXEL_SIZE);
    voxels->setConstructionCompletion(building->getConstructionProgress());

    RenderablePtr renderable;
    renderable.reset(voxels);
    updateRenderableTransform(renderable);
    renderable->initialise(renderContext);
    renderContext->getRenderableSet()->addRenderable(renderable);
    return renderable;
    }
  else
    {
    return ComponentVoxelModel::createRenderable(gameContext, filePath);
    }
  }

