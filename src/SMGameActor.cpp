//
// Created by matt on 10/11/18.
//

#include <GameObjectDefs/DynamicObjectDef.h>
#include <GameObjectDefs/StaticObjectDef.h>
#include "SMGameActor.h"
#include "SMGameContext.h"
#include "GameObjectDefFileHelper.h"

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

  gameContext->getBoundingBoxManager()->removeBoundingBox(boundingBoxID);
  boundingBox.reset();
  }

void SMGameActor::saveActor(XMLElement* element, GameContext* gameContext)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  element->SetAttribute(SL_LINKID, getLinkID());
  element->SetAttribute(SL_GAMEOBJDEF_NAME, gameObjectDef->getUniqueName().c_str());

  // save resources
  XMLElement* resListElem = xmlCreateElement(element, SL_RESOURCELIST);
  for (auto pair : *getStoredResources())
    {
    const int amount = pair.second;
    if (amount == 0)
      continue;
    IGameObjectDefPtr resGameObjDef = smGameContext->getGameObjectFactory()->getGameObjectDef(pair.first);
    if (resGameObjDef)
      {
      XMLElement* resourceElem = xmlCreateElement(resListElem, SL_RESOURCE);
      resourceElem->SetAttribute(SL_NAME, resGameObjDef->getUniqueName().c_str());
      resourceElem->SetAttribute(SL_AMOUNT, amount);
      }
    }

  for (auto behaviour : behaviours)
    {
    const string behaviourName = behaviour->getBehaviourName();
    if (!behaviourName.empty())
      {
      auto behaviourElement = xmlCreateElement(element, xmlCreateBehaviourElemName(behaviourName));
      behaviour->save(this, gameContext, behaviourElement);
      }
    }
  }

void SMGameActor::initialiseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  const GameObjectFactory* gameObjectFactory = smGameContext->getGameObjectFactory();
  uint linkID = 0;
  element->QueryUnsignedAttribute(SL_LINKID, &linkID);
  setLinkID(linkID);

  XMLElement* resListElem = element->FirstChildElement(SL_RESOURCELIST);
  if (resListElem)
    {
    XMLElement* resourceElem = resListElem->FirstChildElement(SL_RESOURCE);
    while (resourceElem)
      {
      const string name = xmlGetStringAttribute(resourceElem, SL_NAME);
      int amount = 0;
      resourceElem->QueryAttribute(SL_AMOUNT, &amount);
      IGameObjectDefPtr resGameObjDef = gameObjectFactory->findGameObjectDef(name);
      if (resGameObjDef && gameObjectFactory->isTypeOrSubType(GameObjectType::resource, resGameObjDef->getType()))
        storeResource(resGameObjDef->getID(), amount);
      else
        mathernogl::logWarning("Loading Actor: Couldn't find resource of name: " + name);
      resourceElem = resListElem->NextSiblingElement(SL_RESOURCE);
      }
    }

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

void SMGameActor::processCommand(const SMActorCommand& command, GameContext* gameContext)
  {
  for (auto behaviour : behaviours)
    {
    if (behaviour->processCommand(this, gameContext, command))
      return;
    }
  }

void SMGameActor::updateBoundingBox()
  {
  if (boundingBox)
    {
    Vector2D midPos = getMidPosition();
    Vector2D halfSize = getSize() * 0.5f;
    double height = 1.3;
    Vector3D lowerBound = Vector3D(midPos.x - halfSize.x, 0, -(midPos.y + halfSize.y));
    Vector3D upperBound = Vector3D(midPos.x + halfSize.x, height, -(midPos.y - halfSize.y));
    boundingBox->setBounds(lowerBound, upperBound);
    }
  }

void SMGameActor::dropAllResources(GameContext* gameContext, Vector2D position)
  {
  for (const auto& pair : *getStoredResources())
    {
    if (pair.second > 0)
      {
      SMGameContext::cast(gameContext)->dropResource(pair.first, pair.second, position);
      takeAllResource(pair.first);
      }
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
    renderable->getTransform()->translate(getPosition3D());
    }
  updateBoundingBox();
  }

void SMStaticActor::setCellPos(Vector2D pos)
  {
  this->cellPos.x = mathernogl::clampf(pos.x, 0, 1);
  this->cellPos.y = mathernogl::clampf(pos.y, 0, 1);
  setGridPos(gridPos);     // sets renderables transform
  updateBoundingBox();
  }

void SMStaticActor::setCellPos(Vector3D pos)
  {
  this->cellPos.x = mathernogl::clampf((float)pos.x, 0, 1);
  this->cellPos.y = mathernogl::clampf((float)-pos.z, 0, 1);
  setGridPos(gridPos);     // sets renderables transform
  updateBoundingBox();
  }

void SMStaticActor::onAttached(GameContext* gameContext)
  {
  SMGameActor::onAttached(gameContext);
  setGridPos(gridPos);     // sets renderables transform

  boundingBox.reset(new BoundingBox());
  boundingBoxID = gameContext->getBoundingBoxManager()->addBoundingBox(boundingBox, getID());
  updateBoundingBox();
  }

Vector2D SMStaticActor::getPosition() const
  {
  return Vector2D(gridPos.x, gridPos.y) + cellPos;
  }

GridXY SMStaticActor::getGridPosition() const
  {
  return gridPos;
  }

Vector2D SMStaticActor::getCellPosition() const
  {
  return cellPos;
  }

Vector3D SMStaticActor::getPosition3D() const
  {
  Vector2D pos = getPosition();
  return Vector3D(pos.x, 0, -pos.y);
  }

void SMStaticActor::saveActor(XMLElement* element, GameContext* gameContext)
  {
  SMGameActor::saveActor(element, gameContext);
  xmlCreateElement(element, SL_GRIDPOS, gridPos);
  xmlCreateElement(element, SL_CELLPOS, cellPos);
  }

void SMStaticActor::initialiseActorFromSave(GameContext* gameContext, XMLElement* element)
  {
  gridPos = xmlGetGridXYValue(element, SL_GRIDPOS);
  cellPos = xmlGetVec2Value(element, SL_CELLPOS);
  SMGameActor::initialiseActorFromSave(gameContext, element);
  }

Vector2D SMStaticActor::getMidPosition() const
  {
  const Vector2D size = getSize();
  if (size.x <= 1 && size.y <= 1)
    return getPosition();
  else
    return getPosition() + (size * 0.5f);
  }

Vector2D SMStaticActor::getSize() const
  {
  if (StaticObjectDef::cast(getDef()))
    return StaticObjectDef::cast(getDef())->getSize();
  }



SMDynamicActor::SMDynamicActor(uint id, const IGameObjectDef* gameObjectDef) : SMGameActor(id, gameObjectDef)
  {}

void SMDynamicActor::onAttached(GameContext* gameContext)
  {
  SMGameActor::onAttached(gameContext);
  updateRenderableTransform();
  }

void SMDynamicActor::onUpdate(GameContext* gameContext)
  {
  SMGameActor::onUpdate(gameContext);
  }

void SMDynamicActor::setPosition(Vector2D position)
  {
  this->position = position;
  updateRenderableTransform();
  updateBoundingBox();
  }

void SMDynamicActor::setRotation(double rotation)
  {
  this->rotation = rotation;
  updateRenderableTransform();
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

Vector2D SMDynamicActor::getMidPosition() const
  {
  return getPosition();
  }

Vector2D SMDynamicActor::getSize() const
  {
  return Vector2D(0.5, 0.5);
  }