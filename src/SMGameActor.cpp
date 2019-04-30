//
// Created by matt on 10/11/18.
//

#include "SMGameActor.h"
#include "SMGameContext.h"
#include "BlueprintFileHelper.h"
#include <UISystem/UIPanel.h>
#include <UISystem/UIProgressBar.h>
#include <UISystem/UIButton.h>
#include <UISystem/UIText.h>

SMGameActor::SMGameActor(uint id, uint blueprintID) : GameActor(id), blueprintID(blueprintID)
  {
  }

void SMGameActor::addComponent(SMComponentPtr component)
  {
  ASSERT(!initialised, "Adding component after onAttached!")
  if (!initialised)
    components.push_back(component);
  }

void SMGameActor::onAttached(GameContext* gameContext)
  {
  setResourceObserverFunc([&]()
    {
    postMessage(gameContext, SMMessage::resourceStorageChanged);
    });

  if (xmlToLoadFrom)
    {
    initialiseActorFromSave(gameContext, xmlToLoadFrom);
    }
  else
    {
    for (auto component : components)
      component->initialise(gameContext);
    }

  boundingBox.reset(new BoundingBox());
  boundingBoxID = gameContext->getBoundingBoxManager()->addBoundingBox(boundingBox, getID());
  updateBoundingBox();

  initialised = true;
  }

void SMGameActor::onUpdate(GameContext* gameContext)
  {
  if (positionChanged)
    sendMessageToComponents(gameContext, SMMessage::actorPositionChanged);
  if (rotationChanged)
    sendMessageToComponents(gameContext, SMMessage::actorRotationChanged);
  positionChanged = rotationChanged = false;

  for (auto component : components)
    component->update(gameContext);
  }

void SMGameActor::onDetached(GameContext* gameContext)
  {
  for (auto component : components)
    component->cleanUp(gameContext);
  components.clear();

  gameContext->getBoundingBoxManager()->removeBoundingBox(boundingBoxID);
  boundingBox.reset();
  initialised = false;
  }

void SMGameActor::saveActor(XMLElement* xmlGameActor, GameContext* gameContext)
  {
  //  save standard game actor properties
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  xmlGameActor->SetAttribute(SL_LINKID, getLinkID());
  xmlGameActor->SetAttribute(SL_TYPE, getTypeName().c_str());
  xmlCreateElement(xmlGameActor, SL_POS, position);
  xmlCreateElement(xmlGameActor, SL_ROT, rotation);

  //  save resources
  XMLElement* resListElem = xmlCreateElement(xmlGameActor, SL_RESOURCELIST);
  for (const ResourceStack& stack : *getResourceStacks())
    {
    const SMGameActorBlueprint* resourceBlueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(stack.id);
    if (resourceBlueprint)
      {
      XMLElement* resourceElem = xmlCreateElement(resListElem, SL_RESOURCE);
      resourceElem->SetAttribute(SL_NAME, resourceBlueprint->name.c_str());
      resourceElem->SetAttribute(SL_AMOUNT, stack.amount);
      }
    }

  //  save components
  for (SMComponentPtr component : components)
    {
    const string elementName = xmlConstructCompElemName(component->getComponentID());
    XMLElement* xmlComponent = xmlCreateElement(xmlGameActor, elementName);
    component->save(gameContext, xmlComponent);
    }
  }

void SMGameActor::initialiseActorFromSave(GameContext* gameContext, XMLElement* xmlGameActor)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  const GameObjectFactory* gameObjectFactory = smGameContext->getGameObjectFactory();
  uint linkID = 0;
  xmlGameActor->QueryUnsignedAttribute(SL_LINKID, &linkID);
  setLinkID(linkID);

  position = xmlGetVec3Value(xmlGameActor, SL_POS);
  rotation = xmlGetDblValue(xmlGameActor, SL_ROT);

  XMLElement* resListElem = xmlGameActor->FirstChildElement(SL_RESOURCELIST);
  if (resListElem)
    {
    XMLElement* resourceElem = resListElem->FirstChildElement(SL_RESOURCE);
    while (resourceElem)
      {
      const string name = xmlGetStringAttribute(resourceElem, SL_NAME);
      int amount = 0;
      resourceElem->QueryAttribute(SL_AMOUNT, &amount);
      const SMGameActorBlueprint* resourceBlueprint = smGameContext->getGameObjectFactory()->findGameActorBlueprint(name);
      if (resourceBlueprint && resourceBlueprint->type == SMGameActorType::resource)
        storeResource(resourceBlueprint->id, amount);
      else
        mathernogl::logWarning("Loading Actor: Couldn't find resource of name: " + name);
      resourceElem = resourceElem->NextSiblingElement(SL_RESOURCE);
      }
    }

  for (SMComponentPtr component : components)
    {
    const string elementName = xmlConstructCompElemName(component->getComponentID());
    XMLElement* xmlComponent = xmlGameActor->FirstChildElement(elementName.c_str());
    component->initialiseFromSaved(gameContext, xmlComponent);
    }
  }

void SMGameActor::updateBoundingBox()
  {
  if (!boundingBox)
    return;

  if (!isPosInCentre)
    {
    boundingBox->setBounds(getPosition3D(), getPosition3D() + size);
    }
  else
    {
    boundingBox->setBounds(getPosition3D() - Vector3D(size.x * 0.5, 0, size.z * 0.5), getPosition3D() + Vector3D(size.x * 0.5, size.y, size.z * 0.5));
    }
  }

void SMGameActor::dropAllResources(GameContext* gameContext)
  {
  dropAllResources(gameContext, getPosition());
  }

void SMGameActor::dropAllResources(GameContext* gameContext, Vector2D position)
  {
  forEachResource([&](uint id, uint amount)
    {
    SMGameContext::cast(gameContext)->dropResource(id, amount, position);
    });
  clearAllResources();
  }

GridXY SMGameActor::getGridPosition() const
  {
  Vector2D position2D = getPosition();
  return GridXY(position2D);
  }

Vector2D SMGameActor::getPosition() const
  {
  return Vector2D(position.x, -position.z);
  }

Vector3D SMGameActor::getPosition3D() const
  {
  return position;
  }

Vector2D SMGameActor::getMidPosition() const
  {
  if (!isPosInCentre)
    return getPosition() + (getSize() * 0.5f);
  else
    return getPosition();
  }

Vector2D SMGameActor::getSize() const
  {
  return Vector2D(size.x, -size.z);
  }

double SMGameActor::getRotation() const
  {
  return rotation;
  }

double SMGameActor::getHeight() const
  {
  return size.y;
  }

void SMGameActor::setSelectable(bool selectable)
  {
  ASSERT(!initialised, "");
  this->isSelectable = selectable;
  }

void SMGameActor::setIsPosInCentre(bool isPosInCentre)
  { 
  ASSERT(!initialised, "");
  this->isPosInCentre = isPosInCentre;
  }

void SMGameActor::setHeight(double height)
  {
  ASSERT(!initialised, "");
  this->size.y = height;
  }

void SMGameActor::setSize2D(Vector2D size)
  {
  ASSERT(!initialised, "");
  this->size.x = size.x;
  this->size.z = -size.y;
  }

void SMGameActor::setPosition(GridXY position)
  {
  setPosition(position.centre());
  }

void SMGameActor::setPosition(Vector2D position)
  {
  setPosition(Vector3D(position.x, 0, -position.y));
  }

void SMGameActor::setPosition(Vector3D position)
  {
  if (this->position != position)
    {
    this->position = position;
    positionChanged = true;
    updateBoundingBox();
    }
  }

void SMGameActor::setRotation(double rotation)
  {
  if (this->rotation != rotation)
    {
    this->rotation = rotation;
    rotationChanged = true;
    updateBoundingBox();
    }
  }

void SMGameActor::setElevation(double elevation)
  {
  setPosition(Vector3D(position.x, elevation, position.z));  
  }

void SMGameActor::sendMessageToComponents(GameContext* gameContext, SMMessage message, void* extra)
  {
  for (SMComponentPtr& component : components)
    component->onMessage(gameContext, message, extra);
  }

bool SMGameActor::getIsPosInCentre() const
  {
  return !isPosInCentre;
  }

bool SMGameActor::getIsSelectable() const
  {
  return isSelectable;
  }

void SMGameActor::postMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  sendMessageToComponents(gameContext, message, extra);
  }

bool SMGameActor::gotComponentType(SMComponentType type) const
  {
  for(SMComponentPtr component : components)
    {
    if(component->getType() == type)
      return true;
    }
  return false;
  }

void SMGameActor::setupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel)
  {
  UIManager* uiManager = gameContext->getUIManager();
  const SMGameActorBlueprint* blueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->getGameActorBlueprint(getBlueprintTypeID());

  UIPanel* selectionPanel = new UIPanel(uiManager->getNextComponentID());
  selectionPanel->setWidthMatchParent(true);
  selectionPanel->setHeightMatchParent(true);
  selectionPanel->setPadding(HUD_BORDER_SIZE, HUD_BORDER_SIZE);
  selectionPanel->setColour(HUD_COL_BG);

  selectionHUD.reset(selectionPanel);
  parentPanel->addChild(selectionHUD);

  UIPanel* iconBorder = new UIPanel(uiManager->getNextComponentID());
  iconBorder->setColour(BTN_UNPRESSED_COL);
  iconBorder->setOffset(Vector2D(10, 10));
  iconBorder->setSize(Vector2D(45));
  iconBorder->setPadding(HUD_BORDER_SIZE, HUD_BORDER_SIZE);
  selectionPanel->addChild(UIComponentPtr(iconBorder));

  UIPanel* icon = new UIPanel(uiManager->getNextComponentID());
  icon->setVerticalAlignment(alignmentCentre);
  icon->setHorizontalAlignment(alignmentCentre);
  icon->setHeightMatchParent(true);
  icon->setWidthMatchParent(true);
  icon->setPadding(BTN_BORDER_SIZE, BTN_BORDER_SIZE);
  icon->setTexture(gameContext->getRenderContext()->getSharedTexture(blueprint->iconPath));
  iconBorder->addChild(UIComponentPtr(icon));

  UIText* nameText = new UIText(uiManager->getNextComponentID());
  nameText->setOffset(Vector2D(65, 10));
  nameText->setSize(Vector2D(170, 50));
  nameText->setFontSize(25);
  nameText->setFontColour(Vector3D(0));
  nameText->showBackground(false);
  nameText->setTextCentreAligned(true, false);
  nameText->setText(blueprint->displayName);
  selectionPanel->addChild(UIComponentPtr(nameText));

  const int paddingY = 10;
  int offsetY = 55 + paddingY;
  int subOffsetY = onSetupSelectionHUD(gameContext, selectionPanel, offsetY);
  if (subOffsetY > 0)
    offsetY += subOffsetY + paddingY;

  for (SMComponentPtr component : components)
    {
    int componentOffsetY = component->onSetupSelectionHUD(gameContext, selectionPanel, offsetY);
    if (componentOffsetY > 0)
      offsetY += componentOffsetY + paddingY;
    }
  }

void SMGameActor::updateSelectionHUD(GameContext* gameContext)
  {
  ASSERT(selectionHUD, "");

  onUpdateSelectionHUD(gameContext);

  for (SMComponentPtr component : components)
    component->onUpdateSelectionHUD(gameContext);
  }

void SMGameActor::removeSelectionHUD(GameContext* gameContext, UIPanel* parentPanel)
  {
  parentPanel->removeChild(selectionHUD->getID());
  selectionHUD.reset();
  }




