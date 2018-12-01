#pragma once

#include <TowOff/GameSystem/GameSystem.h>
#include "SMGameObjectFactory.h"
#include "Grid.h"
#include "SaveLoadFileHelper.h"
#include "Resource.h"

/*
*   
*/

class IGameObjectDef;
class IGameObjectBehaviour;
typedef std::shared_ptr<IGameObjectBehaviour> IGameObjectBehaviourPtr;


class SMGameActor : public GameActor
  {
protected:
  uint smActorLinkID = 0;
  const IGameObjectDef* gameObjectDef;
  std::vector<IGameObjectBehaviourPtr> behaviours;
  RenderablePtr renderable;
  XMLElement* xmlToLoadFrom = nullptr;

public:
  SMGameActor(uint id, const IGameObjectDef* gameObjectDef);
  void setLinkID(uint linkID) { smActorLinkID = linkID; }
  uint getLinkID() const { return smActorLinkID; }
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;
  virtual void saveActor(XMLElement* element);
  void setXMLToLoadFrom(XMLElement* xmlElement){ xmlToLoadFrom = xmlElement; }

  const IGameObjectDef* getDef() const { return gameObjectDef; }
  std::vector<IGameObjectBehaviourPtr>* getBehaviourList() { return &behaviours; }

protected:
  virtual void initialiseActorFromSave(GameContext* gameContext, XMLElement* element);
  };
typedef std::shared_ptr<SMGameActor> SMGameActorPtr;


class SMStaticActor : public SMGameActor
  {
private:
  GridXY gridPos;
  Vector2D cellPos;

public:
  SMStaticActor(uint id, const IGameObjectDef* gameObjectDef);
  virtual void onAttached(GameContext* gameContext) override;

  void setGridPos(GridXY pos);
  void setCellPos (Vector2D pos);
  void setCellPos (Vector3D pos);
  Vector3D getPosition() const;
  GridXY getGridPosition() const;

  virtual void saveActor(XMLElement* element) override;
protected:
  virtual void initialiseActorFromSave(GameContext* gameContext, XMLElement* element) override;
public:

  inline static SMStaticActor* cast(SMGameActor* actor){ return dynamic_cast<SMStaticActor*>(actor); }
  };
typedef std::shared_ptr<SMStaticActor> SMStaticActorPtr;


class SMDynamicActor : public SMGameActor
  {
private:
  Vector2D position;
  double rotation = 0;

  bool gotTarget = false;
  Vector2D targetPosition;
  double speed = 1;   // units per second

public:
  SMDynamicActor(uint id, const IGameObjectDef* gameObjectDef);
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  void setPosition(Vector2D position);
  void setRotation(double rotation);

  bool hasGotTarget() const { return gotTarget; }
  bool hasReachedTarget() const;
  void setTarget(Vector2D target);
  void clearTarget();

  inline static SMDynamicActor* cast(SMGameActor* actor){ return dynamic_cast<SMDynamicActor*>(actor); }

protected:
  void moveToTarget(long deltaTime);
  void updateRenderableTransform();
  };
typedef std::shared_ptr<SMDynamicActor> SMDynamicActorPtr;
