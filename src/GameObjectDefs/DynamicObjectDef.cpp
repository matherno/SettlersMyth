//
// Created by matt on 1/12/18.
//

#include <TowOff/RenderSystem/RenderableMesh.h>
#include <Resources.h>
#include <GameObjectDefFileHelper.h>
#include <TowOff/RenderSystem/RenderableVoxels.h>
#include <Utils.h>
#include <SMGameContext.h>
#include "DynamicObjectDef.h"

bool DynamicObjectDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!GameObjectDefBase::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlRender = xmlGameObjectDef->FirstChildElement(OD_RENDER);
  if (xmlRender)
    {
    const string meshFilePath = xmlGetStringAttribute(xmlRender, OD_MESHFILE);
    const string voxelsFilePath = xmlGetStringAttribute(xmlRender, OD_VOXELFILE);
    if (!meshFilePath.empty())
      {
      isMeshRenderable = true;
      renderableFilePath = RES_DIR + meshFilePath;
      }
    else if (!voxelsFilePath.empty())
      {
      isMeshRenderable = false;
      renderableFilePath = RES_DIR + voxelsFilePath;
      }
    }

  return true;
  }

RenderablePtr DynamicObjectDef::constructRenderable(RenderContext* renderContext, const Vector3D& translation, uint meshIdx) const
  {
  if (renderableFilePath.empty())
    return nullptr;

  RenderablePtr ptr;
  if (isMeshRenderable)
    {
    RenderableMesh* renderable = new RenderableMesh(renderContext->getNextRenderableID());
    renderable->setMeshStorage(renderContext->getSharedMeshStorage(renderableFilePath));
    renderable->setDrawStyleVertColours();
    ptr.reset(renderable);
    }
  else
    {
    RenderableVoxels* renderable = new RenderableVoxels(renderContext->getNextRenderableID(), DRAW_STAGE_NO_SHADOW_CASTING);
    renderable->setVoxelStorage(renderContext->getSharedVoxelStorage(renderableFilePath));
    renderable->setVoxelSize(DYNAMIC_VOXEL_SIZE);
    ptr.reset(renderable);
    }
  ptr->initialise(renderContext);
  renderContext->getRenderableSet()->addRenderable(ptr);
  return ptr;
  }

SMGameActorPtr DynamicObjectDef::createGameActor(GameContext* gameContext) const
  {
  SMDynamicActor* actor = new SMDynamicActor(gameContext->getNextActorID(), this);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }

