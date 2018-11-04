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

struct TOGameState
  {
  Vector3D cameraFocalPos;
  double cameraZoomFactor = 0;
  double cameraRotation = 0;
  std::vector<TowerState> towers;
  };

class TOGameSaveLoad
  {
public:
  static bool loadGame(TOGameState* state, string filePath);
  static bool saveGame(const TOGameState* state, string filePath);
  static bool verifyState(const TOGameState* state);
  };
