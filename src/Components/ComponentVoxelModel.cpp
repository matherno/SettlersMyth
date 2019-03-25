//
// Created by matt on 25/02/19.
//

#include <RenderSystem/RenderableVoxels.h>
#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include <Resources.h>
#include "ComponentVoxelModel.h"
#include "RenderSystem/VoxelBatchManager.h"


/*
*   ComponentVoxelModelBlueprint
*/

bool ComponentVoxelModelBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  staticModel = (xmlGetStringAttribute(xmlComponent, OD_STATICMODEL) == "true");

  modelFilePaths.clear();
  XMLElement* xmlModel = xmlComponent->FirstChildElement(OD_MODEL);
  while (xmlModel)
    {
    const string voxelFilePath = xmlGetStringAttribute(xmlModel, OD_VOXELFILE);
    if (!voxelFilePath.empty())
      modelFilePaths.push_back(RES_DIR + voxelFilePath);
    xmlModel = xmlModel->NextSiblingElement(OD_MODEL);
    }

  if (modelFilePaths.empty())
    {
    *errorMsg = "No voxel models found!";
    return false;
    }

  return true;
  }

SMComponentPtr ComponentVoxelModelBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  if (!modelFilePaths.empty())
    return SMComponentPtr(new ComponentVoxelModel(actor, type, this));
  return nullptr;
  }





/*
*   ComponentVoxelModel
*/

ComponentVoxelModel::ComponentVoxelModel(const SMGameActorPtr& actor, SMComponentType type, const ComponentVoxelModelBlueprint* blueprint)
    : SMComponent(actor, type), blueprint(blueprint)
  {
  ASSERT(!blueprint->modelFilePaths.empty(), "");
  }

void ComponentVoxelModel::initialise(GameContext* gameContext)
  {
  voxelModelIdx = (uint) mathernogl::RandomGenerator::randomInt(0, (int) blueprint->modelFilePaths.size() - 1);
  createRenderable(gameContext, blueprint->modelFilePaths[voxelModelIdx]);
  }

void ComponentVoxelModel::initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent)
  {
  voxelModelIdx = std::min(xmlComponent->UnsignedAttribute(SL_MODELIDX), (uint)blueprint->modelFilePaths.size() - 1);
  createRenderable(gameContext, blueprint->modelFilePaths[voxelModelIdx]);
  }

void ComponentVoxelModel::update(GameContext* gameContext)
  {
  }

void ComponentVoxelModel::cleanUp(GameContext* gameContext)
  {
  if (voxelModel)
    {
    voxelModel->cleanUp(gameContext->getRenderContext());
    gameContext->getRenderContext()->getRenderableSet()->removeRenderable(voxelModel->getID());
    voxelModel.reset();
    }
  }

void ComponentVoxelModel::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  switch (message)
    {
    case SMMessage::actorRotationChanged:
    case SMMessage::actorPositionChanged:
      updateRenderableTransform();
    }
  }

void ComponentVoxelModel::save(GameContext* gameContext, XMLElement* xmlComponent)
  {
  xmlComponent->SetAttribute(SL_MODELIDX, voxelModelIdx);
  }

void ComponentVoxelModel::updateRenderableTransform()
  {
  if (!blueprint->staticModel)
    {
    voxelModel->getTransform()->setIdentityMatrix();
    voxelModel->getTransform()->rotate(0, 1, 0, getActor()->getRotation());
    voxelModel->getTransform()->translate(getActor()->getPosition3D());
    }
  }

void ComponentVoxelModel::createRenderable(GameContext* gameContext, const string& filePath)
  {
  RenderContext* renderContext = gameContext->getRenderContext();
  if (blueprint->staticModel)
    {
    voxelModel = renderContext->getVoxelBatchManager()->createVoxelModelInstance(renderContext, filePath, getActor()->getPosition3D());
    }
  else
    {
    RenderableVoxels* renderable = new RenderableVoxels(renderContext->getNextRenderableID(), DRAW_STAGE_NO_SHADOW_CASTING);
    renderable->setVoxelStorage(renderContext->getSharedVoxelStorage(filePath));
    renderable->setVoxelSize(DYNAMIC_VOXEL_SIZE);
    voxelModel.reset(renderable);
    updateRenderableTransform();
    }
  voxelModel->initialise(renderContext);
  renderContext->getRenderableSet()->addRenderable(voxelModel);
  }
