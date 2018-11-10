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

  auto xmlRender = xmlGameObjectDef->FirstChildElement(OD_RENDER);
  if (xmlRender)
    meshFilePath = RES_DIR + xmlGetStringAttribute(xmlRender, OD_MESHFILE);
  return true;
  }


RenderablePtr StaticObjectDef::constructRenderable(RenderContext* renderContext) const
  {
  if (meshFilePath.empty())
    return nullptr;

  RenderableMesh* renderable = new RenderableMesh(renderContext->getNextRenderableID());
  renderable->setMeshStorage(renderContext->getSharedMeshStorage(meshFilePath));
  renderable->initialise(renderContext);
  renderable->setDrawStyleVertColours();
  renderContext->getRenderableSet()->addRenderable(RenderablePtr(renderable));
  return RenderablePtr(renderable);
  }


SMGameActorPtr StaticObjectDef::createGameActor(GameContext* gameContext) const
  {
  SMStaticActor* actor = new SMStaticActor(gameContext->getNextActorID(), this);
  return SMGameActorPtr(actor);
  }
