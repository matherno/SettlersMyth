#pragma once

#include <GameSystem/GameSystem.h>

/*
*   Actor that removes itself after the given number of milliseconds
*   Will cleanup and remove its stored renderables and game actors when it does
*/

class TimeToLiveActor : public GameActor
  {
protected:
  long timeToLive = 2000;
  std::list<RenderablePtr> renderables;
  std::list<GameActorPtr> actors;
  bool firstUpdate = true;

public:
  TimeToLiveActor(uint id, long timeToLive = 200);
  void setTimeToLive(long timeToLive) { this->timeToLive = timeToLive; }
  void addRenderable(RenderablePtr renderable){ renderables.push_back(renderable); }
  void addGameActor(GameActorPtr actor){ actors.push_back(actor); }
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;
  };
