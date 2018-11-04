#pragma once

#include <RenderSystem/RenderSystem.h>
#include <GameSystem/GameConfig.h>

/*
*   
*/

class SMSettings
  {
private:
  GameConfigManagerPtr manager;
  std::shared_ptr<GameConfigValueMap> activeValueMap;
  uint idxScreenWidth;
  uint idxScreenHeight;
  uint idxAntiAliasing;
  uint idxCamPanSpeed;
  uint idxCamRotSpeed;
  uint idxFullscreen;
  uint idxEnableEdgePan;

public:
  SMSettings();
  virtual ~SMSettings() {};

  void initialise();
  std::shared_ptr<GameConfigValueMap> createValueMap();
  void setActiveValueMap(std::shared_ptr<GameConfigValueMap> map);
  const GameConfigValueMap* getActiveValueMap() const { return activeValueMap.get(); }

  const GameConfigManager* getConfigManager() const;
  void readSettingsFile(GameConfigValueMap* map) const;
  void writeSettingsFile(const GameConfigValueMap* map) const;

  uint getWindowWidth() const;
  uint getWindowHeight() const;
  bool useAntiAliasing() const;
  bool useFullscreen() const;
  float getCameraPanSpeed() const;
  float getCameraRotSpeed() const;
  bool enableScreenEdgePan() const;
  };
