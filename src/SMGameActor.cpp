//
// Created by matt on 10/11/18.
//

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


SMResourceActor::SMResourceActor(uint id, const IGameObjectDef* gameObjectDef) : SMGameActor(id, gameObjectDef)
  {}
