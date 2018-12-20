//
// Created by matt on 10/11/18.
//

#include <TowOff/RenderSystem/RenderableMesh.h>
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

  meshFilePathList.clear();
  auto xmlRender = xmlGameObjectDef->FirstChildElement(OD_RENDER);
  if (xmlRender)
    {
    XMLElement* xmlMeshFile = xmlRender->FirstChildElement(OD_MESHFILE);
    while (xmlMeshFile)
      {
      string meshFilePath = RES_DIR + xmlMeshFile->GetText();
      if (!meshFilePath.empty())
        meshFilePathList.push_back(meshFilePath);
      xmlMeshFile = xmlMeshFile->NextSiblingElement(OD_MESHFILE);
      }
    }

  if (meshFilePathList.size() == 0)
    {
    *errorMsg = "Static object missing mesh file";
    return false;
    }

  return true;
  }


RenderablePtr StaticObjectDef::constructRenderable(RenderContext* renderContext, uint meshIdx) const
  {
  string meshFilePath = "";
  if (meshIdx < meshFilePathList.size())
    meshFilePath = meshFilePathList[meshIdx];
  if (meshFilePath.empty())
    return nullptr;

  RenderableMesh* renderable = new RenderableMesh(renderContext->getNextRenderableID());
  renderable->setMeshStorage(renderContext->getSharedMeshStorage(meshFilePath));
  renderable->initialise(renderContext);
  renderable->setDrawStyleVertColours();
  RenderablePtr ptr(renderable);
  renderContext->getRenderableSet()->addRenderable(ptr);
  return ptr;
  }


SMGameActorPtr StaticObjectDef::createGameActor(GameContext* gameContext) const
  {
  SMStaticActor* actor = new SMStaticActor(gameContext->getNextActorID(), this);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }

