//
// Created by matt on 3/12/17.
//

#include <GameSystem/InputCodes.h>
#include "SMInputHandler.h"
#include "SMGameContext.h"
#include "SaveLoadDlg.h"

SMInputHandler::SMInputHandler(uint id, const Vector3D& focalPosition, float zoomOffset, float rotation, float pitch)
  : InputHandler(id), focalPosition(focalPosition), zoomOffset(zoomOffset), rotation(rotation), pitch(pitch)
  {
  }

void SMInputHandler::refreshCamera(Camera* camera)
  {
  *camera->getWorldToCamera() = mathernogl::matrixTranslate(focalPosition * -1) * rotationMatrix * mathernogl::matrixTranslate(0, 0, -zoomOffset);
  camera->setValid(false);
  cameraNeedsRefresh = false;
  }

void SMInputHandler::onAttached(GameContext* gameContext)
  {
  Camera* camera = gameContext->getCamera();
  refreshRotationMatrix();
  refreshCamera(camera);
  *camera->getCameraToClip() = mathernogl::matrixPerspective(1, gameContext->getRenderContext()->getWindow()->getAspectRatio(), -1, -300);
  camera->setValid(false);
  }

void SMInputHandler::onDetached(GameContext* gameContext)
  {

  }

void SMInputHandler::onUpdate(GameContext* gameContext)
  {
  performMouseCameraMovement(gameContext);
  if (cameraNeedsRefresh)
    {
    Camera* camera = gameContext->getCamera();
    refreshRotationMatrix();
    refreshCamera(camera);
    *camera->getCameraToClip() = mathernogl::matrixPerspective(1, gameContext->getRenderContext()->getWindow()->getAspectRatio(), -1, -300);
    camera->setValid(false);
    }
  }

bool SMInputHandler::onKeyHeld(GameContext* gameContext, uint key)
  {
  SMGameContext* toGameContext = SMGameContext::cast(gameContext);
  char character = getCharFromKeyCode(key);
  Vector3D translation(0);
  float rotationDelta = 0;
  float speed = panSpeed * gameContext->getDeltaTime() * toGameContext->getSettings()->getCameraPanSpeed() * 0.001f;
  float rotateSpeed = yawSpeed * gameContext->getDeltaTime() * toGameContext->getSettings()->getCameraRotSpeed() * 0.001f;
  speed *= zoomOffset / maxZoom;

  switch(character)
    {
    case 'W':
      translation.set(0, 0, -speed);
      break;
    case 'A':
      translation.set(-speed, 0, 0);
      break;
    case 'S':
      translation.set(0, 0, speed);
      break;
    case 'D':
      translation.set(speed, 0, 0);
      break;
    case 'Q':
      rotationDelta = -rotateSpeed;
      break;
    case 'E':
      rotationDelta = rotateSpeed;
      break;
    }

  if (translation.magnitude() > 0 || rotationDelta != 0)
    {
    if (rotationDelta != 0)
      {
      rotation += rotationDelta;
      refreshRotationMatrix();
      }
    translation *= mathernogl::matrixYaw(rotation);
    Vector3D newFocalPosition = focalPosition + translation;
    focalPosition = newFocalPosition;
    refreshCamera(gameContext->getCamera());
    return true;
    }
  return false;
  }

void SMInputHandler::refreshRotationMatrix()
  {
  rotationMatrix = mathernogl::matrixYaw(-rotation) * mathernogl::matrixPitch(-pitch);
  }

bool SMInputHandler::onKeyPressed(GameContext* gameContext, uint key)
  {
  if (key == KEY_ESC)
    {
    SMGameContext::cast(gameContext)->displayPauseMenu();
    return true;
    }

  switch(getCharFromKeyCode(key))
    {
    case 'P':
      paused = !paused;
      gameContext->setPaused(paused);
      return true;
    case 'T':
      SMGameContext::cast(gameContext)->getHUDHandler()->toggleDebugPanel();
      return true;
    }
  return false;
  }

bool SMInputHandler::onMousePressed(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  if (button == MOUSE_LEFT)
    {
    SMGameContext* smGameContext = SMGameContext::cast(gameContext);
    Vector2D terrainPos = smGameContext->terrainHitTest(mouseX, mouseY);

    if (!currentUnit)
      {
      auto unitGameDef = smGameContext->getGameObjectFactory()->findGameObjectDef("Settler");
      currentUnit = smGameContext->createSMGameActor(unitGameDef->getID(), terrainPos);
      }
    else
      {
      if (!gameContext->getInputManager()->isKeyDown(KEY_LCTRL))
        Unit::cast(currentUnit.get())->clearOrders(gameContext);
      Unit::cast(currentUnit.get())->pushOrder(IUnitOrderPtr(new UnitOrderMoveToTarget(terrainPos)));
      }

    }
  return false;
  }

bool SMInputHandler::onMouseHeld(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
//  if (button == MOUSE_LEFT)
//    {
//    WorldItemSelectionManager* selectionManager = SMGameContext::cast(gameContext)->getSelectionManager();
//    if (!selectionManager->isMouseDragging())
//      selectionManager->onStartMouseDrag(gameContext, mouseX, mouseY);
//    else
//      selectionManager->onUpdateMouseDrag(gameContext, mouseX, mouseY);
//    return true;
//    }
  return false;
  }

bool SMInputHandler::onMouseReleased(GameContext* gameContext, uint button, uint mouseX, uint mouseY)
  {
  SMGameContext* toGameContext = SMGameContext::cast(gameContext);
  if (button == MOUSE_LEFT)
    {
//    WorldItemSelectionManager* selectionManager = toGameContext->getSelectionManager();
//    bool isCtrlDown = gameContext->getInputManager()->isKeyDown(KEY_LCTRL);
//    if (selectionManager->isMouseDragging())
//      {
//      if(selectionManager->onFinishMouseDrag(gameContext, mouseX, mouseY, isCtrlDown))
//        return true;
//      }
//    return selectionManager->onWorldClick(gameContext, mouseX, mouseY, isCtrlDown);
    }
  else if (button == MOUSE_MIDDLE)
    {
//    Vector3D worldPos = SMGameContext::cast(gameContext)->terrainHitTest(mouseX, mouseY);
//    toGameContext->createDepositGroup(worldPos, 2.5, 10);
//    return true;
    }
  return false;
  }

bool SMInputHandler::onMouseScroll(GameContext* gameContext, double scrollOffset, uint mouseX, uint mouseY)
  {
  zoomOffset -= scrollOffset * zoomSpeed;
  zoomOffset = mathernogl::clampf(zoomOffset, minZoom, maxZoom);
  refreshCamera(gameContext->getCamera());
  return true;
  }

void SMInputHandler::performMouseCameraMovement(GameContext* gameContext)
  {
  SMGameContext* toGameContext = SMGameContext::cast(gameContext);
  if (!toGameContext->getSettings()->enableScreenEdgePan())
    return;

  const uint edgeCamMoveWidth = 10;
  const Vector2D screenSize = gameContext->getRenderContext()->getWindow()->getSize();
  const Vector2D mousePos = gameContext->getInputManager()->getMousePos();

  Vector3D camTranslation;
  if (mousePos.x < edgeCamMoveWidth)
    camTranslation.x -= 1;
  else if (mousePos.x > (screenSize.x - edgeCamMoveWidth))
    camTranslation.x += 1;
  if (mousePos.y < edgeCamMoveWidth)
    camTranslation.z -= 1;
  else if (mousePos.y > (screenSize.y - edgeCamMoveWidth))
    camTranslation.z += 1;

  if (camTranslation.magnitude() > 0)
    {
    camTranslation.makeUniform();
    camTranslation *= mousePanSpeed * gameContext->getDeltaTime() * toGameContext->getSettings()->getCameraPanSpeed() * 0.001f;
    camTranslation *= mathernogl::matrixYaw(rotation);
    Vector3D newFocalPosition = focalPosition + camTranslation;
    focalPosition = newFocalPosition;
    refreshCamera(gameContext->getCamera());
    }
  }
