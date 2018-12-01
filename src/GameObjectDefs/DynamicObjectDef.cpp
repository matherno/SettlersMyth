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

  auto xmlUnit = xmlGameObjectDef->FirstChildElement(OD_UNIT);
  if (xmlUnit)
    xmlUnit->QueryAttribute(OD_SPEED, &speed);
  return true;

  return true;
  }

RenderablePtr DynamicObjectDef::constructRenderable(RenderContext* renderContext) const
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

SMGameActorPtr DynamicObjectDef::createGameActor(GameContext* gameContext) const
  {
  SMDynamicActor* actor = new SMDynamicActor(gameContext->getNextActorID(), this);
  createActorBehaviours(actor->getBehaviourList());
  return SMGameActorPtr(actor);
  }
