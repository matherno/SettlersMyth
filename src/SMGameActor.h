#pragma once

#include <TowOff/GameSystem/GameSystem.h>
#include "SMGameObjectFactory.h"
#include "Grid.h"
#include "SaveLoadFileHelper.h"
#include "ResourceStorage.h"
#include "SMActorCommand.h"

/*
*   
*/

class IGameObjectDef;
class IGameObjectBehaviour;

typedef std::shared_ptr<IGameObjectBehaviour> IGameObjectBehaviourPtr;

class SMGameActor : public GameActor, public ResourceStorage
  {
protected:
  uint smActorLinkID = 0;
  const IGameObjectDef* gameObjectDef;
  std::vector<IGameObjectBehaviourPtr> behaviours;
  RenderablePtr renderable;
  XMLElement* xmlToLoadFrom = nullptr;
  BoundingBoxPtr boundingBox;
  uint boundingBoxID = 0;

public:
  SMGameActor(uint id, const IGameObjectDef* gameObjectDef);
  void setLinkID(uint linkID) { smActorLinkID = linkID; }
  uint getLinkID() const { return smActorLinkID; }
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;
  virtual void saveActor(XMLElement* element, GameContext* gameContext);

  const IGameObjectDef* getDef() const { return gameObjectDef; }
  std::vector<IGameObjectBehaviourPtr>* getBehaviourList() { return &behaviours; }
  virtual Vector2D getMidPosition() const = 0;
  virtual Vector2D getSize() const = 0;
  virtual double getRotation() const { return 0; };
  void setXMLToLoadFrom(XMLElement* xmlElement){ xmlToLoadFrom = xmlElement; }
  void processCommand(const SMActorCommand& command, GameContext* gameContext);
  void dropAllResources(GameContext* gameContext, Vector2D position);
  void updateBoundingBox();
  BoundingBoxPtr getBoundingBox() { return boundingBox; }

protected:
  virtual void initialiseActorFromSave(GameContext* gameContext, XMLElement* element);
  };

typedef std::shared_ptr<SMGameActor> SMGameActorPtr;
typedef std::weak_ptr<SMGameActor> SMGameActorWkPtr;

class SMStaticActor : public SMGameActor
  {
private:
  GridXY gridPos;
  Vector2D cellPos;

public:
  SMStaticActor(uint id, const IGameObjectDef* gameObjectDef);
  virtual void onAttached(GameContext* gameContext) override;
  virtual void saveActor(XMLElement* element, GameContext* gameContext) override;

  void setGridPos(GridXY pos);
  void setCellPos (Vector2D pos);
  void setCellPos (Vector3D pos);
  Vector2D getCellPosition() const;
  GridXY getGridPosition() const;
  Vector2D getPosition() const;
  Vector3D getPosition3D() const;
  virtual Vector2D getMidPosition() const override;
  virtual Vector2D getSize() const override;

  inline static SMStaticActor* cast(SMGameActor* actor){ return dynamic_cast<SMStaticActor*>(actor); }

protected:
  virtual void initialiseActorFromSave(GameContext* gameContext, XMLElement* element) override;
  };
typedef std::shared_ptr<SMStaticActor> SMStaticActorPtr;


class SMDynamicActor : public SMGameActor
  {
private:
  Vector2D position;
  double elevation = 0;
  double rotation = 0;

public:
  SMDynamicActor(uint id, const IGameObjectDef* gameObjectDef);
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  void setPosition(Vector2D position);
  void setPosition(Vector3D position);
  void setRotation(double rotation);
  void setElevation(double elevation);
  Vector2D getPosition() const { return position; }
  double getElevation() const { return elevation; };
  virtual Vector2D getMidPosition() const override;
  virtual Vector2D getSize() const override;
  virtual double getRotation() const override { return rotation; }

  inline static SMDynamicActor* cast(SMGameActor* actor){ return dynamic_cast<SMDynamicActor*>(actor); }

protected:
  void updateRenderableTransform();
  };
typedef std::shared_ptr<SMDynamicActor> SMDynamicActorPtr;
