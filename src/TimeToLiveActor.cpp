//
// Created by matt on 21/12/17.
//

#include "TimeToLiveActor.h"

TimeToLiveActor::TimeToLiveActor(uint id, long timeToLive) : GameActor(id), timeToLive(timeToLive)
  {}

void TimeToLiveActor::onAttached(GameContext* gameContext)
  {
  }

void TimeToLiveActor::onUpdate(GameContext* gameContext)
  {
  timeToLive -= gameContext->getDeltaTime();
  if (!firstUpdate && timeToLive <= 0)
    gameContext->removeActor(getID());
  firstUpdate = false;
  }

void TimeToLiveActor::onDetached(GameContext* gameContext)
  {
  RenderContext* renderContext = gameContext->getRenderContext();
  for (GameActorPtr actor : actors)
    {
    gameContext->removeActor(actor->getID());
    }
  for (RenderablePtr renderable : renderables)
    {
    renderable->cleanUp(renderContext);
    renderContext->getRenderableSet()->removeRenderable(renderable->getID());
    }
  }
