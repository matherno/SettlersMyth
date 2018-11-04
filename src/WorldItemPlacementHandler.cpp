//
// Created by matt on 16/02/18.
//

#include <RenderSystem/RenderableMesh.h>
#include <RenderSystem/RenderableSetImpl.h>
#include <GameSystem/InputCodes.h>
#include "WorldItemPlacementHandler.h"
#include "SMGameContext.h"

TowerPlacementHandler::TowerPlacementHandler(uint id, uint towerTypeID) : InputHandler(id), towerTypeID(towerTypeID)
  {}

void TowerPlacementHandler::onAttached(GameContext* gameContext)
  {
  RenderContext* renderContext = gameContext->getRenderContext();
  towerHighlight = renderContext->createRenderableSet();
  renderContext->getRenderableSet()->addRenderable(towerHighlight);

//  PhasingMesh* mesh = new PhasingMesh(renderContext->getNextRenderableID(), DRAW_STAGE_OVERLAY);
//  mesh->setMeshStorage(renderContext->getSharedMeshStorage(type->baseMeshFilePath));
//  mesh->initialise(renderContext);
//  towerHighlight->addRenderable(RenderablePtr(mesh));
  }

void TowerPlacementHandler::onDetached(GameContext* gameContext)
  {
  RenderContext* renderContext = gameContext->getRenderContext();
  towerHighlight->forEachChild([renderContext](RenderablePtr child)
    {
    renderContext->getRenderableSet()->removeRenderable(child->getID());
    child->cleanUp(renderContext);
    });
  renderContext->getRenderableSet()->removeRenderable(towerHighlight->getID());
  towerHighlight->cleanUp(renderContext);
  }

bool TowerPlacementHandler::onMouseReleased(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
//  TOGameContext* toGameContext = TOGameContext::cast(gameContext);
//  if (button == MOUSE_LEFT)
//    {
//    if (isHighlightPosValid)
//      {
//      const TowerType* towerType = TowerFactory::getTowerType(towerTypeID);
//      TowerPtr tower = toGameContext->createTower(towerTypeID, towerHighlightPos, true);
//
//      if (!gameContext->getInputManager()->isKeyDown(KEY_LCTRL))
//        {
//        if (endHandlerCallback)
//          endHandlerCallback();
//        }
//      else
//        {
//        //  placing another tower
//        toGameContext->displayAllRelayRanges();
//        }
//      }
//    }
//  else if (button == MOUSE_RIGHT)
//    endHandlerCallback();
  return true;
  }

bool TowerPlacementHandler::onMouseMove(GameContext* gameContext, uint mouseX, uint mouseY, uint prevMouseX, uint prevMouseY)
  {
//  TOGameContext* toGameContext = TOGameContext::cast(gameContext);
//  bool isLand = false;
//  Vector3D terrainPos = toGameContext->terrainHitTest(mouseX, mouseY, &isLand);
//  towerHighlight->getTransform()->translate(terrainPos - towerHighlightPos);
//  towerHighlightPos = terrainPos;
  return true;
  }

bool TowerPlacementHandler::onMousePressed(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  return true;
  }

bool TowerPlacementHandler::onMouseHeld(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  return true;
  }
