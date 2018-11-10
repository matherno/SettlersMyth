//
// Created by matt on 16/02/18.
//

#include <RenderSystem/RenderableMesh.h>
#include <RenderSystem/RenderableSetImpl.h>
#include <GameSystem/InputCodes.h>
#include <GameObjectDefs/BuildingDef.h>
#include "WorldItemPlacementHandler.h"
#include "SMGameContext.h"

BuildingPlacementHandler::BuildingPlacementHandler(uint id, uint buildingDefID) : InputHandler(id), buildingDefID(buildingDefID)
  {}

void BuildingPlacementHandler::onAttached(GameContext* gameContext)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  auto gameObjDef = smGameContext->getGameObjectFactory()->getGameObjectDef(buildingDefID);
  if (gameObjDef)
    {
    const auto buildingDef = dynamic_cast<const BuildingDef*>(gameObjDef.get());
    if (buildingDef)
      {
      buildingSizeX = buildingDef->getSizeX();
      buildingSizeY = buildingDef->getSizeY();

      auto renderContext = gameContext->getRenderContext();
      RenderableLineStrips* lineStrips = new RenderableLineStrips(renderContext->getNextRenderableID());
      lineStrips->setLineWidth(3);
      buildingOutline.reset(lineStrips);
      buildingOutline->initialise(renderContext);
      renderContext->getRenderableSet()->addRenderable(buildingOutline);
      updateOutline();
      }
    }

  }

void BuildingPlacementHandler::onDetached(GameContext* gameContext)
  {
  if (buildingOutline)
    {
    buildingOutline->cleanUp(gameContext->getRenderContext());
    gameContext->getRenderContext()->getRenderableSet()->removeRenderable(buildingOutline->getID());
    buildingOutline.reset();
    }
  }

bool BuildingPlacementHandler::onMouseReleased(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  if (button == MOUSE_LEFT)
    {
    if (isOutlinePosValid)
      {
      smGameContext->createSMGameActor(buildingDefID, buildingPlacementPos);
      if (!gameContext->getInputManager()->isKeyDown(KEY_LCTRL))
        {
        if (endHandlerCallback)
          endHandlerCallback();
        }
      else
        {
        isOutlinePosValid = false;
        updateOutline();
        }
      }
    }
  else if (button == MOUSE_RIGHT)
    endHandlerCallback();
  return true;
  }

bool BuildingPlacementHandler::onMouseMove(GameContext* gameContext, uint mouseX, uint mouseY, uint prevMouseX, uint prevMouseY)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  Vector2D terrainPos = smGameContext->terrainHitTest(mouseX, mouseY);
  buildingPlacementPos = terrainPos;
  buildingPlacementPos.x -= std::floor((double)buildingSizeX / 2.0);
  buildingPlacementPos.y -= std::floor((double)buildingSizeY / 2.0);

  isOutlinePosValid = smGameContext->isRegionClear(buildingPlacementPos, GridXY(buildingSizeX, buildingSizeY));
  if (buildingOutline)
    {
    buildingOutline->getTransform()->setIdentityMatrix();
    buildingOutline->getTransform()->translate(std::floor(buildingPlacementPos.x), 0.1, -std::floor(buildingPlacementPos.y));
    updateOutline();
    }
  return true;
  }


bool BuildingPlacementHandler::onMouseHeld(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  return true;
  }

void BuildingPlacementHandler::updateOutline()
  {
  if (!buildingOutline)
    return;
  RenderableLineStrips* lineStrips = dynamic_cast<RenderableLineStrips*>(buildingOutline.get());
  if (!lineStrips)
    return;

  lineStrips->clearLineStrips();
  for (double y : std::vector<double> { 0.01, 0.5 })
    {
    lineStrips->startLineStrip(isOutlinePosValid ? outlineColourValid : outlineColourInvalid);
    lineStrips->addPoint(Vector3D(0, y, 0));
    lineStrips->addPoint(Vector3D(buildingSizeX, y, 0));
    lineStrips->addPoint(Vector3D(buildingSizeX, y, -buildingSizeY));
    lineStrips->addPoint(Vector3D(0, y, -buildingSizeY));
    lineStrips->addPoint(Vector3D(0, y, 0));
    lineStrips->finishLineStrip();
    }
  }
