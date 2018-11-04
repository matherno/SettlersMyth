#pragma once

#include <GameSystem/GameSystem.h>
#include <RenderSystem/RenderableLines.h>

/*
*   
*/

class TowerPlacementHandler : public InputHandler
  {
private:
  const uint towerTypeID;
  RenderableSetPtr towerHighlight;
  Vector3D towerHighlightPos;
  bool isHighlightPosValid = false;
  CallbackFunc endHandlerCallback;
  float previewColourAlpha = 0.5f;

public:
  TowerPlacementHandler(uint id, uint towerTypeID);
  void setEndHandlerCallback(CallbackFunc func){ endHandlerCallback = func; }

private:
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;
  virtual bool onMousePressed(GameContext* gameContext, uint button, uint mouseX, uint mouseY) override;
  virtual bool onMouseHeld(GameContext* gameContext, uint button, uint mouseX, uint mouseY) override;
  virtual bool onMouseReleased(GameContext* gameContext, uint button, uint mouseX, uint mouseY) override;
  virtual bool onMouseMove(GameContext* gameContext, uint mouseX, uint mouseY, uint prevMouseX, uint prevMouseY) override;
  };
