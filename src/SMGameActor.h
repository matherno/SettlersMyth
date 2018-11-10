#pragma once

#include <TowOff/GameSystem/GameSystem.h>
#include "SMGameObjectFactory.h"
#include "Grid.h"

/*
*   
*/

class IGameObjectDef;
class IGameObjectBehaviour;
typedef std::shared_ptr<IGameObjectBehaviour> IGameObjectBehaviourPtr;


class SMGameActor : public GameActor
  {
protected:
  const IGameObjectDef* gameObjectDef;
  std::vector<IGameObjectBehaviourPtr> behaviours;
  RenderablePtr renderable;

public:
  SMGameActor(uint id, const IGameObjectDef* gameObjectDef);
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;

  const IGameObjectDef* getDef() const { return gameObjectDef; }
  std::vector<IGameObjectBehaviourPtr>* getBehaviourList() { return &behaviours; }
  };
typedef std::shared_ptr<SMGameActor> SMGameActorPtr;


class SMStaticActor : public SMGameActor
  {
private:
  GridXY gridPos;

public:
  SMStaticActor(uint id, const IGameObjectDef* gameObjectDef);
  virtual void onAttached(GameContext* gameContext) override;

  void setPos(GridXY pos);
  };
typedef std::shared_ptr<SMStaticActor> SMStaticActorPtr;


class SMResourceActor : public SMGameActor
  {
private:
  Vector2D position;
  double rotation = 0;

public:
  SMResourceActor(uint id, const IGameObjectDef* gameObjectDef);


  };
typedef std::shared_ptr<SMResourceActor> SMResourceActorPtr;
