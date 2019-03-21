//
// Created by matt on 16/02/18.
//

#include <RenderSystem/RenderableMesh.h>
#include <RenderSystem/RenderableSetImpl.h>
#include <GameSystem/InputCodes.h>
#include <GameActorTypes/GameActorBuilding.h>
#include "WorldItemPlacementHandler.h"
#include "SMGameContext.h"

WorldItemPlacementHandler::WorldItemPlacementHandler(uint id, uint buildingBlueprintID) : InputHandler(id), buildingBlueprintID(buildingBlueprintID)
  {}

void WorldItemPlacementHandler::onAttached(GameContext* gameContext)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  const SMGameActorBlueprint* blueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(buildingBlueprintID);
  ASSERT(blueprint, "");
  if (blueprint)
    {
    buildingSizeX = blueprint->gridSize.x;
    buildingSizeY = blueprint->gridSize.y;

    auto renderContext = gameContext->getRenderContext();
    RenderableLineStrips* lineStrips = new RenderableLineStrips(renderContext->getNextRenderableID());
    lineStrips->setLineWidth(3);
    buildingOutline.reset(lineStrips);
    buildingOutline->initialise(renderContext);
    renderContext->getRenderableSet()->addRenderable(buildingOutline);
    updateOutline();
    }
  }

void WorldItemPlacementHandler::onDetached(GameContext* gameContext)
  {
  if (buildingOutline)
    {
    buildingOutline->cleanUp(gameContext->getRenderContext());
    gameContext->getRenderContext()->getRenderableSet()->removeRenderable(buildingOutline->getID());
    buildingOutline.reset();
    }
  }

bool WorldItemPlacementHandler::onMouseReleased(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  if (button == MOUSE_LEFT)
    {
    if (isOutlinePosValid)
      {
      SMGameActorPtr gameActor = smGameContext->createSMGameActor(buildingBlueprintID, buildingPlacementPos);

      if (GameActorBuilding* building = GameActorBuilding::cast(gameActor.get()))
        {
        for (int i = 0; i < 3; ++i)
          {
          SMGameActorPtr unit = smGameContext->createSMGameActor(smGameContext->getGameObjectFactory()->findGameActorBlueprint("Settler")->id, building->getEntryPosition());
          ASSERT(unit, "");
          GameActorUnitPtr unitPtr = std::dynamic_pointer_cast<GameActorUnit>(unit);
          building->attachUnit(unitPtr);
          }
        }

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

bool WorldItemPlacementHandler::onMouseMove(GameContext* gameContext, uint mouseX, uint mouseY, uint prevMouseX, uint prevMouseY)
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  Vector2D terrainPos = smGameContext->terrainHitTest(mouseX, mouseY);
  buildingPlacementPos = terrainPos;
  buildingPlacementPos.x -= std::floor((double)buildingSizeX / 2.0);
  buildingPlacementPos.y -= std::floor((double)buildingSizeY / 2.0);

  isOutlinePosValid = smGameContext->getGridMapHandler()->isRegionClear(buildingPlacementPos, GridXY(buildingSizeX, buildingSizeY));
  if (buildingOutline)
    {
    buildingOutline->getTransform()->setIdentityMatrix();
    buildingOutline->getTransform()->translate(std::floor(buildingPlacementPos.x), 0.1, -std::floor(buildingPlacementPos.y));
    updateOutline();
    }
  return true;
  }

bool WorldItemPlacementHandler::onKeyPressed(GameContext* gameContext, uint key)
  {
  if (key == KEY_ESC)
    {
    endHandlerCallback();
    return true;
    }
    
  return false;
  }

bool WorldItemPlacementHandler::onMouseHeld(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  return true;
  }

void WorldItemPlacementHandler::updateOutline()
  {
  if (!buildingOutline)
    return;
  RenderableLineStrips* lineStrips = dynamic_cast<RenderableLineStrips*>(buildingOutline.get());
  if (!lineStrips)
    return;

  lineStrips->clearLineStrips();
  for (double y : std::vector<double> { 0.01, 1 })
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
