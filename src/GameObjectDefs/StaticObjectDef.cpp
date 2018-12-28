//
// Created by matt on 10/11/18.
//

#include <TowOff/RenderSystem/RenderableMesh.h>
#include <TowOff/RenderSystem/RenderableVoxels.h>
#include <Utils.h>
#include "StaticObjectDef.h"
#include "GameObjectDefFileHelper.h"
#include "Resources.h"

bool StaticObjectDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!GameObjectDefBase::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlGrid = xmlGameObjectDef->FirstChildElement(OD_GRID);
  if (!xmlGrid)
    {
    *errorMsg = "Static object missing 'grid' element";
    return false;
    }

  size.x = xmlGrid->IntAttribute(OD_X, 1);
  size.y = xmlGrid->IntAttribute(OD_Y, 1);

  renderList.clear();
  XMLElement* xmlRender = xmlGameObjectDef->FirstChildElement(OD_RENDER);
  while (xmlRender)
    {
    XMLElement* xmlRenderPart = xmlRender->FirstChildElement(OD_RENDERPART);
    const string meshFilePath = xmlGetStringAttribute(xmlRenderPart, OD_MESHFILE);
    const string voxelFilePath = xmlGetStringAttribute(xmlRenderPart, OD_VOXELFILE);

    DefRenderItem renderItem;
    if (!meshFilePath.empty())
      {
      renderItem.filePath = RES_DIR + meshFilePath;
      renderItem.isMeshFile = true;
      }
    else if (!voxelFilePath.empty())
      {
      renderItem.filePath = RES_DIR + voxelFilePath;
      renderItem.isMeshFile = false;
      }
    if (!renderItem.filePath.empty())
      renderList.push_back(renderItem);

    xmlRender = xmlRender->NextSiblingElement(OD_RENDER);
    }

  if (renderList.size() == 0)
    {
    *errorMsg = "Static object missing a render item";
    return false;
    }

  return true;
  }


RenderablePtr StaticObjectDef::constructRenderable(RenderContext* renderContext, uint meshIdx) const
  {
  string filePath = "";
  if (meshIdx < renderList.size())
    filePath = renderList[meshIdx].filePath;
  if (filePath.empty())
    return nullptr;
  const bool isMeshFile = renderList[meshIdx].isMeshFile;

  RenderablePtr ptr;
  if (isMeshFile)
    {
    RenderableMesh* renderable = new RenderableMesh(renderContext->getNextRenderableID());
    renderable->setMeshStorage(renderContext->getSharedMeshStorage(filePath));
    renderable->setDrawStyleVertColours();
    ptr.reset(renderable);
    }
  else
    {
    RenderableVoxels* renderable = new RenderableVoxels(renderContext->getNextRenderableID());
    renderable->setVoxelStorage(renderContext->getSharedVoxelStorage(filePath));
    renderable->setVoxelSize(VOXEL_SIZE);
    ptr.reset(renderable);
    }

  ptr->initialise(renderContext);
  renderContext->getRenderableSet()->addRenderable(ptr);
  return ptr;
  }


SMGameActorPtr StaticObjectDef::createGameActor(GameContext* gameContext) const
  {
  SMStaticActor* actor = new SMStaticActor(gameContext->getNextActorID(), this);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }

