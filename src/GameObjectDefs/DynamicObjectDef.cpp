//
// Created by matt on 1/12/18.
//

#include <TowOff/RenderSystem/RenderableMesh.h>
#include <Resources.h>
#include <GameObjectDefFileHelper.h>
#include "DynamicObjectDef.h"

bool DynamicObjectDef::loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg)
  {
  if(!GameObjectDefBase::loadFromXML(xmlGameObjectDef, errorMsg))
    return false;

  auto xmlRender = xmlGameObjectDef->FirstChildElement(OD_RENDER);
  if (xmlRender)
    meshFilePath = RES_DIR + xmlGetStringAttribute(xmlRender, OD_MESHFILE);

  return true;
  }

RenderablePtr DynamicObjectDef::constructRenderable(RenderContext* renderContext, uint meshIdx) const
  {
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

SMGameActorPtr DynamicObjectDef::createGameActor(GameContext* gameContext) const
  {
  SMDynamicActor* actor = new SMDynamicActor(gameContext->getNextActorID(), this);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }
