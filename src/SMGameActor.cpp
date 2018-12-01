//
// Created by matt on 10/11/18.
//

#include <GameObjectDefs/DynamicObjectDef.h>
#include "SMGameActor.h"

SMGameActor::SMGameActor(uint id, const IGameObjectDef* gameObjectDef) : GameActor(id), gameObjectDef(gameObjectDef)
  {}

void SMGameActor::onAttached(GameContext* gameContext)
  {
  renderable = gameObjectDef->constructRenderable(gameContext->getRenderContext());
  if (xmlToLoadFrom)
    {
    initialiseActorFromSave(gameContext, xmlToLoadFrom);
    }
  else
    {
    for (auto behaviour : behaviours)
      behaviour->initialise(this, gameContext);
    }
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

void SMGameActor::saveActor(XMLElement* element)
  {
  element->SetAttribute(SL_LINKID, getLinkID());
  element->SetAttribute(SL_GAMEOBJDEF_NAME, gameObjectDef->getUniqueName().c_str());
  for (auto behaviour : behaviours)
    {
    const string behaviourName = behaviour->getBehaviourName();
    if (!behaviourName.empty())
      {
      auto behaviourElement = xmlCreateElement(element, xmlCreateBehaviourElemName(behaviourName));
      behaviour->save(this, behaviourElement);
      }
    }
  }

void SMGameActor::initialiseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  uint linkID = 0;
  element->QueryUnsignedAttribute(SL_LINKID, &linkID);
  setLinkID(linkID);

  for (auto behaviour : behaviours)
    {
    const string behaviourName = behaviour->getBehaviourName();
    const string behaviourElemName = xmlCreateBehaviourElemName(behaviourName);
    XMLElement* xmlBehaviourElement = nullptr;
    if (!behaviourName.empty())
      xmlBehaviourElement = element->FirstChildElement(behaviourElemName.c_str());

    if (xmlBehaviourElement)
      behaviour->initialiseFromSaved(this, gameContext, xmlBehaviourElement);
    else
      behaviour->initialise(this, gameContext);
    }

  }



SMStaticActor::SMStaticActor(uint id, const IGameObjectDef* gameObjectDef) : SMGameActor(id, gameObjectDef)
  {}

void SMStaticActor::setGridPos(GridXY pos)
  {
  this->gridPos = pos;
  if (renderable)
    {
    renderable->getTransform()->setIdentityMatrix();
    renderable->getTransform()->translate(getPosition());
    }
  }

void SMStaticActor::setCellPos(Vector2D pos)
  {
  this->cellPos.x = mathernogl::clampf(pos.x, 0, 1);
  this->cellPos.y = mathernogl::clampf(pos.y, 0, 1);
  setGridPos(gridPos);     // sets renderables transform
  }

void SMStaticActor::setCellPos(Vector3D pos)
  {
  this->cellPos.x = mathernogl::clampf((float)pos.x, 0, 1);
  this->cellPos.y = mathernogl::clampf((float)-pos.z, 0, 1);
  setGridPos(gridPos);     // sets renderables transform
  }

void SMStaticActor::onAttached(GameContext* gameContext)
  {
  SMGameActor::onAttached(gameContext);
  setGridPos(gridPos);     // sets renderables transform
  }

Vector3D SMStaticActor::getPosition() const
  {
  return Vector3D(cellPos.x + gridPos.x, 0, -(cellPos.y + gridPos.y));
  }

GridXY SMStaticActor::getGridPosition() const
  {
  return gridPos;
  }

void SMStaticActor::saveActor(XMLElement* element)
  {
  SMGameActor::saveActor(element);
  xmlCreateElement(element, SL_GRIDPOS, gridPos);
  xmlCreateElement(element, SL_CELLPOS, cellPos);
  }

void SMStaticActor::initialiseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  gridPos = xmlGetGridXYValue(element, SL_GRIDPOS);
  cellPos = xmlGetVec2Value(element, SL_CELLPOS);
  SMGameActor::initialiseActorFromSave(gameContext, element);
  }





SMDynamicActor::SMDynamicActor(uint id, const IGameObjectDef* gameObjectDef) : SMGameActor(id, gameObjectDef)
  {}

void SMDynamicActor::onAttached(GameContext* gameContext)
  {
  SMGameActor::onAttached(gameContext);
  updateRenderableTransform();
  speed = DynamicObjectDef::cast(gameObjectDef)->getSpeed();
  }

void SMDynamicActor::onUpdate(GameContext* gameContext)
  {
  if (gotTarget && !hasReachedTarget())
    moveToTarget(gameContext->getDeltaTime());
  SMGameActor::onUpdate(gameContext);
  }

void SMDynamicActor::setPosition(Vector2D position)
  {
  this->position = position;
  updateRenderableTransform();
  }

void SMDynamicActor::setRotation(double rotation)
  {
  this->rotation = rotation;
  updateRenderableTransform();
  }

bool SMDynamicActor::hasReachedTarget() const
  {
  if (!gotTarget)
    return false;
  return fabs(position.x - targetPosition.x) < 1e-10 && fabs(position.y - targetPosition.y) < 1e-10;
  }

void SMDynamicActor::setTarget(Vector2D target)
  {
  gotTarget = true;
  targetPosition = target;
  }

void SMDynamicActor::clearTarget()
  {
  gotTarget = false;
  }

void SMDynamicActor::moveToTarget(long deltaTime)
  {
  double maxDistance = ((double)deltaTime / 1000.0) * speed;
  Vector2D posToTarget = targetPosition - position;
  if (posToTarget.magnitude() < maxDistance)
    setPosition(targetPosition);
  else
    setPosition(position + posToTarget.getUniform() * (float)maxDistance);
  setRotation(-1 * mathernogl::ccwAngleBetween(Vector2D(0, 1), posToTarget.getUniform()));
  }

void SMDynamicActor::updateRenderableTransform()
  {
  if (renderable)
    {
    renderable->getTransform()->setIdentityMatrix();
    renderable->getTransform()->rotate(0, 1, 0, rotation);
    renderable->getTransform()->translate(position.x, 0, -position.y);
    }
  }
