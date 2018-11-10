#pragma once

#include <RenderSystem/RenderSystem.h>
#include <RenderSystem/HeightMapFactory.h>


struct TowerState
  {
  Vector3D position;
  double rotation = 0;
  uint type = 0;
  uint health = 0;
  uint energy = 0;
  bool underConstruction = false;
  };

struct SMGameState
  {
  Vector3D cameraFocalPos;
  double cameraZoomFactor = 0;
  double cameraRotation = 0;
  std::vector<TowerState> towers;
  };

class SMGameSaveLoad
  {
public:
  static bool loadGame(SMGameState* state, string filePath);
  static bool saveGame(const SMGameState* state, string filePath);
  static bool verifyState(const SMGameState* state);
  };
