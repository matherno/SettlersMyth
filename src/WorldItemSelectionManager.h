#pragma once

#include <GameSystem/GameSystem.h>
#include <RenderSystem/RenderableLineBoxes.h>
#include <RenderSystem/RenderableLineStrips.h>
#include "SMGameActor.h"

/*
*   
*/

class WorldItemSelectionManager : public GameActor
  {
private:
  mathernogl::MappedList<SMGameActorPtr> selectedActors;
  std::shared_ptr<RenderableLineBoxes> selectionBoxRenderables;
  std::map<uint, uint> actorSelectionBoxesMap;
  Vector3D selectionColour = Vector3D(0.7);
  float selectionAlpha = 0.75f;
  float selectionBoxLineWidth = 1;
  float selectionRectLineWidth = 1;
  std::shared_ptr<RenderableLineStrips> dragSelectionRect;
  bool isSelectionDragging = false;
  Vector2D startDragPoint;

public:
  WorldItemSelectionManager(uint id);

  //  returns true if at least one tower was selected
  bool onWorldClick(GameContext* gameContext, uint mouseX, uint mouseY, bool isCtrlClick);
  void onStartMouseDrag(GameContext* gameContext, uint mouseX, uint mouseY);
  void onUpdateMouseDrag(GameContext* gameContext, uint mouseX, uint mouseY);

  // returns false if the drag wasn't significant enough to do a drag selection
  bool onFinishMouseDrag(GameContext* gameContext, uint mouseX, uint mouseY, bool isCtrlDown);
  bool isMouseDragging() const;

  void deselectAll(GameContext* gameContext);
  void deselectTower(GameContext* gameContext, uint id);
  void selectActor(GameContext* gameContext, SMGameActorPtr actor);
  void selectActorScreenRect(GameContext* gameContext, const Vector2D& screenPoint1, const Vector2D& screenPoint2);
  bool isActorSelected(uint actorID) const;
  SMGameActorPtr getFirstSelectedActor();
  const mathernogl::MappedList<SMGameActorPtr>* getSelectedActors() const { return &selectedActors; }
  uint selectionCount() const { return selectedActors.count(); }

  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;

  /*
   *  Corners...
   *      A             B
   *      *-------------*
   *     /              |
   *    /               |
   *   *----------------*
   *  D                 C
   */
  static bool isPointWithinTrapezoid(const Vector2D& point, const Vector2D& cornerA, const Vector2D& cornerB, const Vector2D& cornerC, const Vector2D& cornerD);
protected:
  void expandBoundingBox(BoundingBox* box);
  bool isDragSignificant(Vector2D startPoint, Vector2D endPoint);
  };
