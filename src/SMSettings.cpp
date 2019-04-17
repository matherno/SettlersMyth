//
// Created by matt on 19/04/18.
//

#include "SMSettings.h"

#define CONFIG_FILE "smconfig.ini"

SMSettings::SMSettings()
  {
  manager.reset(new GameConfigManager());
  idxScreenWidth = manager->addOption(GameConfigManager::typeInt, "Screen Width", "screenx", "graphics", "1366");
  idxScreenHeight = manager->addOption(GameConfigManager::typeInt, "Screen Height", "screeny", "graphics", "768");
  idxAntiAliasing = manager->addOption(GameConfigManager::typeBool, "Anti-Aliasing", "antialiasing", "graphics", "true");
  idxCamPanSpeed = manager->addOption(GameConfigManager::typeFloat, "Camera Pan Speed Factor", "campanspeed", "input", "1");
  idxCamRotSpeed = manager->addOption(GameConfigManager::typeFloat, "Camera Rotate Speed Factor", "camrotspeed", "input", "1");
  idxFullscreen = manager->addOption(GameConfigManager::typeBool, "Fullscreen", "fullscreen", "graphics", "false");
  idxEnableEdgePan = manager->addOption(GameConfigManager::typeBool, "Screen Edge Pan", "screenedgepan", "input", "true");
  idxShadows = manager->addOption(GameConfigManager::typeBool, "Shadows", "shadows", "graphics", "true");
  }

void SMSettings::initialise()
  {
  activeValueMap = createValueMap();
  if(std::ifstream(CONFIG_FILE))
    readSettingsFile(activeValueMap.get());
  else
    writeSettingsFile(activeValueMap.get());
  }

std::shared_ptr<GameConfigValueMap> SMSettings::createValueMap()
  {
  std::shared_ptr<GameConfigValueMap> map = std::make_shared<GameConfigValueMap>(manager);
  map->setDefaultValues();
  return map;
  }

void SMSettings::setActiveValueMap(std::shared_ptr<GameConfigValueMap> map)
  {
  activeValueMap = map;
  }

const GameConfigManager* SMSettings::getConfigManager() const
  {
  return manager.get();
  }

void SMSettings::readSettingsFile(GameConfigValueMap* map) const
  {
  map->readIniFile(CONFIG_FILE);
  }

void SMSettings::writeSettingsFile(const GameConfigValueMap* map) const
  {
  map->writeIniFile(CONFIG_FILE);
  }

uint SMSettings::getWindowWidth() const
  {
  int width = 800;
  activeValueMap->getIntValue(idxScreenWidth, &width);
  return (uint)width;
  }

uint SMSettings::getWindowHeight() const
  {
  int width = 800;
  activeValueMap->getIntValue(idxScreenHeight, &width);
  return (uint)width;
  }

bool SMSettings::useAntiAliasing() const
  {
  bool aa = true;
  activeValueMap->getBoolValue(idxAntiAliasing, &aa);
  return aa;
  }

bool SMSettings::useFullscreen() const
  {
  bool fullscreen = false;
  activeValueMap->getBoolValue(idxFullscreen, &fullscreen);
  return fullscreen;
  }

float SMSettings::getCameraPanSpeed() const
  {
  float speed = 1;
  activeValueMap->getFloatValue(idxCamPanSpeed, &speed);
  return speed;
  }

float SMSettings::getCameraRotSpeed() const
  {
  float speed = 1;
  activeValueMap->getFloatValue(idxCamRotSpeed, &speed);
  return speed;
  }

bool SMSettings::enableScreenEdgePan() const
  {
  bool enable = false;
  activeValueMap->getBoolValue(idxEnableEdgePan, &enable);
  return enable;
  }

bool SMSettings::enableShadows() const
  {
  bool enable = false;
  activeValueMap->getBoolValue(idxShadows, &enable);
  return enable;
  }


