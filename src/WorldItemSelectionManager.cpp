//
// Created by matt on 7/04/18.
//

#include "WorldItemSelectionManager.h"
#include "SMGameContext.h"

#define SELECTION_BOX_EXPANSION_FACTOR 1.0
#define SELECTION_BOX_Y_OFFSET -0.05
#define DRAG_SELECT_TOLERANCE  5

WorldItemSelectionManager::WorldItemSelectionManager(uint id) : GameActor(id)
  {}

void WorldItemSelectionManager::onAttached(GameContext* gameContext)
  {
  selectionBoxRenderables.reset(new RenderableLineBoxes(gameContext->getRenderContext()->getNextRenderableID(), DRAW_STAGE_OPAQUE_AFTER_EDGE));
  selectionBoxRenderables->setLineColour(selectionColour);
  selectionBoxRenderables->setAlpha(selectionAlpha);
  selectionBoxRenderables->setLineWidth(selectionBoxLineWidth);
  selectionBoxRenderables->initialise(gameContext->getRenderContext());
  gameContext->getRenderContext()->getRenderableSet()->addRenderable(selectionBoxRenderables);
  }

void WorldItemSelectionManager::onUpdate(GameContext* gameContext)
  {

  }

void WorldItemSelectionManager::onDetached(GameContext* gameContext)
  {
  selectionBoxRenderables->cleanUp(gameContext->getRenderContext());
  gameContext->getRenderContext()->getRenderableSet()->removeRenderable(selectionBoxRenderables->getID());
  deselectAll(gameContext);
  if (dragSelectionRect)
    {
    dragSelectionRect->cleanUp(gameContext->getRenderContext());
    gameContext->getRenderContext()->getRenderableSet()->removeRenderable(dragSelectionRect->getID());
    dragSelectionRect.reset();
    }
  }

bool WorldItemSelectionManager::onWorldClick(GameContext* gameContext, uint mouseX, uint mouseY, bool isCtrlClick)
  {
  if (!isCtrlClick)
    deselectAll(gameContext);

  SMGameContext* toGameContext = SMGameContext::cast(gameContext);
  if (gameContext->getBoundingBoxManager()->boundingBoxPicked())
    {
    uint pickedActorID = (uint)gameContext->getBoundingBoxManager()->getPickedBoundingBoxMeta();
    if (!isActorSelected(pickedActorID))
      {
      if (SMGameActorPtr pickedActor = toGameContext->getSMGameActor(pickedActorID))
        selectActor(gameContext, pickedActor);
      }
    else if (isCtrlClick)
      {
      deselectTower(gameContext, pickedActorID);
      }
    return true;
    }
  return false;
  }

void WorldItemSelectionManager::onStartMouseDrag(GameContext* gameContext, uint mouseX, uint mouseY)
  {
  isSelectionDragging = true;
  dragSelectionRect.reset(new RenderableLineStrips(gameContext->getRenderContext()->getNextRenderableID(), true, DRAW_STAGE_OVERLAY));
  dragSelectionRect->initialise(gameContext->getRenderContext());
  dragSelectionRect->setLineWidth(selectionRectLineWidth);
  gameContext->getRenderContext()->getRenderableSet()->addRenderable(dragSelectionRect);
  startDragPoint.set(mouseX, mouseY);
  }

void WorldItemSelectionManager::onUpdateMouseDrag(GameContext* gameContext, uint mouseX, uint mouseY)
  {
  if (isSelectionDragging && isDragSignificant(startDragPoint, Vector2D(mouseX, mouseY)))
    {
    dragSelectionRect->clearLineStrips();
    dragSelectionRect->startLineStrip(selectionColour, selectionAlpha);
    dragSelectionRect->addPoint(startDragPoint);
    dragSelectionRect->addPoint(Vector2D(mouseX, startDragPoint.y));
    dragSelectionRect->addPoint(Vector2D(mouseX, mouseY));
    dragSelectionRect->addPoint(Vector2D(startDragPoint.x, mouseY));
    dragSelectionRect->addPoint(startDragPoint);
    dragSelectionRect->finishLineStrip();
    }
  }

bool WorldItemSelectionManager::onFinishMouseDrag(GameContext* gameContext, uint mouseX, uint mouseY, bool isCtrlDown)
  {
  if (isSelectionDragging)
    {
    dragSelectionRect->cleanUp(gameContext->getRenderContext());
    gameContext->getRenderContext()->getRenderableSet()->removeRenderable(dragSelectionRect->getID());
    dragSelectionRect.reset();
    isSelectionDragging = false;

    if (isDragSignificant(startDragPoint, Vector2D(mouseX, mouseY)))
      {
      if (!isCtrlDown)
        deselectAll(gameContext);
      selectActorScreenRect(gameContext, startDragPoint, Vector2D(mouseX, mouseY));
      return true;
      }
    }
  return false;
  }

bool WorldItemSelectionManager::isMouseDragging() const
  {
  return isSelectionDragging;
  }

void WorldItemSelectionManager::deselectAll(GameContext* gameContext)
  {
  actorSelectionBoxesMap.clear();
  selectionBoxRenderables->clearBoxes();
  selectedActors.clear();
  }

void WorldItemSelectionManager::deselectTower(GameContext* gameContext, uint actorID)
  {
  // remove actor selection box
  if (actorSelectionBoxesMap.count(actorID) > 0)
    {
    selectionBoxRenderables->removeBox(actorSelectionBoxesMap[actorID]);
    actorSelectionBoxesMap.erase(actorID);
    }
  if (selectedActors.contains(actorID))
    selectedActors.remove(actorID);
  }

void WorldItemSelectionManager::selectActor(GameContext* gameContext, SMGameActorPtr actor)
  {
  selectedActors.add(actor, actor->getID());

  // create selection box if one doesn't already exist for this actor
  if (actorSelectionBoxesMap.count(actor->getID()) == 0)
    {
    BoundingBox boundingBox(*actor->getBoundingBox());
    expandBoundingBox(&boundingBox);
    uint boxID = selectionBoxRenderables->addBox(boundingBox.getLowerBound(), boundingBox.getUpperBound());
    actorSelectionBoxesMap[actor->getID()] = boxID;
    }
  }

void WorldItemSelectionManager::selectActorScreenRect(GameContext* gameContext, const Vector2D& screenPoint1, const Vector2D& screenPoint2)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  Vector2D terrainHitPoints[] =
    {
      smGameContext->terrainHitTest(screenPoint1.x, screenPoint1.y),
      smGameContext->terrainHitTest(screenPoint2.x, screenPoint1.y),
      smGameContext->terrainHitTest(screenPoint2.x, screenPoint2.y),
      smGameContext->terrainHitTest(screenPoint1.x, screenPoint2.y),
    };

  smGameContext->forEachSMActor(GameObjectType::staticObject, [this, gameContext, terrainHitPoints](SMGameActorPtr actor)
    {
    Vector2D actorPos = actor->getMidPosition();
    if (isPointWithinTrapezoid(actorPos, terrainHitPoints[0], terrainHitPoints[1], terrainHitPoints[2], terrainHitPoints[3]))
      selectActor(gameContext, actor);
    });
  }

bool WorldItemSelectionManager::isActorSelected(uint actorID) const
  {
  return selectedActors.contains(actorID);
  }


SMGameActorPtr WorldItemSelectionManager::getFirstSelectedActor()
  {
  if (selectedActors.count() > 0)
    return *selectedActors.getList()->begin();
  return nullptr;
  }

void WorldItemSelectionManager::expandBoundingBox(BoundingBox* box)
  {
  Vector3D expansion = box->getSize() * (SELECTION_BOX_EXPANSION_FACTOR - 1.0) * 0.5;
  Vector3D min = box->getLowerBound() - expansion + Vector3D(0, SELECTION_BOX_Y_OFFSET, 0);
  Vector3D max = box->getUpperBound() + expansion;
  box->setBounds(min, max);
  }

bool WorldItemSelectionManager::isDragSignificant(Vector2D startPoint, Vector2D endPoint)
  {
  return (startPoint - endPoint).magnitude() > DRAG_SELECT_TOLERANCE;
  }

float crossProduct(const Vector2D& a, const Vector2D& b)
  {
  return (a.x * b.y) - (a.y * b.x);
  }

/*
 *  Corners...
 *      A             B
 *      *-------------*
 *     /              |
 *    /               |
 *   *----------------*
 *  D                 C
 */
bool WorldItemSelectionManager::isPointWithinTrapezoid(const Vector2D& point, const Vector2D& cornerA, const Vector2D& cornerB, const Vector2D& cornerC, const Vector2D& cornerD)
  {
  Vector2D aToP = point - cornerA;
  Vector2D bToP = point - cornerB;
  Vector2D dToP = point - cornerD;

  Vector2D aToB = cornerB - cornerA;
  Vector2D aToD = cornerD - cornerA;
  Vector2D dToC = cornerC - cornerD;
  Vector2D bToC = cornerC - cornerB;

  if (crossProduct(point - cornerA, cornerB - cornerA) * crossProduct(point - cornerD, cornerC - cornerD) > 0)
    return false;
  if (crossProduct(point - cornerA, cornerD - cornerA) * crossProduct(point - cornerB, cornerC - cornerB) > 0)
    return false;
  return true;
  }

