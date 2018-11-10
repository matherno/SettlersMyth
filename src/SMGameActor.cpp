//
// Created by matt on 10/11/18.
//

#include "SMGameActor.h"

SMGameActor::SMGameActor(uint id, const IGameObjectDef* gameObjectDef) : GameActor(id), gameObjectDef(gameObjectDef)
  {}

void SMGameActor::onAttached(GameContext* gameContext)
  {
  renderable = gameObjectDef->constructRenderable(gameContext->getRenderContext());
  for (auto behaviour : behaviours)
    behaviour->initialise(this, gameContext);
  }

void SMGameActor::onUpdate(GameContext* gameContext)
  {
  for (auto behaviour : behaviours)
    behaviour->update(this, gameContext);
  }

void SMGameActor::onDetached(GameContext* gameContext)
  {
  if (renderable)
    {
    renderable->cleanUp(gameContext->getRenderContext());
    gameContext->getRenderContext()->getRenderableSet()->removeRenderable(renderable->getID());
    }
  for (auto behaviour : behaviours)
    behaviour->cleanUp(this, gameContext);
  }




SMStaticActor::SMStaticActor(uint id, const IGameObjectDef* gameObjectDef) : SMGameActor(id, gameObjectDef)
  {}

void SMStaticActor::setPos(GridXY pos)
  {
  gridPos = pos;
  if (renderable)
    {
    renderable->getTransform()->setIdentityMatrix();
    renderable->getTransform()->translate(gridPos);
    }
  }

void SMStaticActor::onAttached(GameContext* gameContext)
  {
  SMGameActor::onAttached(gameContext);
  setPos(gridPos);     // sets renderables transform
  }


SMResourceActor::SMResourceActor(uint id, const IGameObjectDef* gameObjectDef) : SMGameActor(id, gameObjectDef)
  {}
