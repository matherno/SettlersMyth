#pragma once

#include <GameSystem/GameSystem.h>
#include <RenderSystem/RenderableLines.h>
#include "Grid.h"

/*
*   
*/

class WorldItemPlacementHandler : public InputHandler
  {
private:
  const uint buildingDefID;
  RenderablePtr buildingOutline;
  GridXY buildingPlacementPos;
  bool isOutlinePosValid = false;
  CallbackFunc endHandlerCallback;
  Vector3D outlineColourValid = Vector3D(0.1, 0.5, 0.1);
  Vector3D outlineColourInvalid = Vector3D(0.5, 0.1, 0.1);
  int buildingSizeX = 1;
  int buildingSizeY = 1;

public:
  WorldItemPlacementHandler(uint id, uint buildingDefID);
  void setEndHandlerCallback(CallbackFunc func){ endHandlerCallback = func; }

  virtual void onAttached(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;
  virtual bool onMouseHeld(GameContext* gameContext, uint button, uint mouseX, uint mouseY) override;
  virtual bool onMouseReleased(GameContext* gameContext, uint button, uint mouseX, uint mouseY) override;
  virtual bool onMouseMove(GameContext* gameContext, uint mouseX, uint mouseY, uint prevMouseX, uint prevMouseY) override;

protected:
  void updateOutline();
  };
