//
// Created by matt on 25/03/18.
//

#include "SMGameSaveLoad.h"

#define SAVE_FILE_VERIFY_LINE "TOSaveFile"
#define CURRENT_VERSION 1.0
#define CAMERA_POS_X              "camera_pos_x"
#define CAMERA_POS_Y              "camera_pos_y"
#define CAMERA_POS_Z              "camera_pos_z"
#define CAMERA_ZOOM               "camera_zoom"
#define CAMERA_ROT                "camera_rot"
#define TOWER_LIST_START          "towerlist_start"
#define TOWER_LIST_END            "towerlist_end"
#define TOWER_START               "tower_start"
#define TOWER_END                 "tower_end"
#define TOWER_TYPE                "type"
#define TOWER_POS_X               "pos_x"
#define TOWER_POS_Y               "pos_y"
#define TOWER_POS_Z               "pos_z"
#define TOWER_ROT                 "rot"
#define TOWER_ENERGY              "energy"
#define TOWER_HEALTH              "health"
#define TOWER_CONSTR              "construction"

bool lineStartsWith(const std::string& line, const std::string& attribute)
  {
  return line.find(attribute) != std::string::npos;
  }

bool getAttributeValue(const std::string& line, const std::string& attribute, std::string& value)
  {
  unsigned long lineIdx = line.find(attribute);
  if (lineIdx != std::string::npos)
    {
    lineIdx = line.find(' ');
    if (lineIdx != std::string::npos)
      {
      value = line.substr(lineIdx + 1);
      return true;
      }
    }
  return false;
  }

bool getAttributeValue(const std::string& line, const std::string& attribute, double& value)
  {
  string stringValue = "";
  if(getAttributeValue(line, attribute, stringValue))
    {
    value = atof(stringValue.c_str());
    return true;
    }
  return false;
  }

bool getAttributeValue(const std::string& line, const std::string& attribute, uint& value)
  {
  string stringValue = "";
  if(getAttributeValue(line, attribute, stringValue))
    {
    value = (uint)std::max(atoi(stringValue.c_str()), 0);
    return true;
    }
  return false;
  }

bool getAttributeValue(const std::string& line, const std::string& attribute, bool& value)
  {
  uint uintValue = 0;
  if(getAttributeValue(line, attribute, uintValue))
    {
    value = (bool)uintValue;
    return true;
    }
  return false;
  }

bool getAttributeValue(const std::string& line, const std::string& attribute, long& value)
  {
  string stringValue = "";
  if(getAttributeValue(line, attribute, stringValue))
    {
    value = atoi(stringValue.c_str());
    return true;
    }
  return false;
  }

bool readTowerList(std::ifstream& inFile, std::vector<TowerState>& towerList)
  {
  std::string line;
  TowerState towerState;
  while (std::getline(inFile, line))
    {
    if (lineStartsWith(line, TOWER_LIST_END))
      return true;

    getAttributeValue(line, TOWER_TYPE, towerState.type);
    getAttributeValue(line, TOWER_POS_X, towerState.position.x);
    getAttributeValue(line, TOWER_POS_Y, towerState.position.y);
    getAttributeValue(line, TOWER_POS_Z, towerState.position.z);
    getAttributeValue(line, TOWER_ROT, towerState.rotation);
    getAttributeValue(line, TOWER_ENERGY, towerState.energy);
    getAttributeValue(line, TOWER_HEALTH, towerState.health);
    getAttributeValue(line, TOWER_CONSTR, towerState.underConstruction);

    if (lineStartsWith(line, TOWER_END))
      towerList.emplace_back(towerState);
    }
  return true;
  }

bool SMGameSaveLoad::loadGame(SMGameState* state, string filePath)
  {
  if (!state)
    return false;

  string errMsg = "Failed to load game from " + filePath + ". ";

  std::ifstream infile(filePath);
  std::string line;
  if (!std::getline(infile, line))
    {
    mathernogl::logError(errMsg + "Can not read file.");
    return false;
    }

  if(line != SAVE_FILE_VERIFY_LINE)
    {
    mathernogl::logError(errMsg + "Not a TO Save File");
    return false;
    }

  while (std::getline(infile, line))
    {
    getAttributeValue(line, CAMERA_POS_X, state->cameraFocalPos.x);
    getAttributeValue(line, CAMERA_POS_Y, state->cameraFocalPos.y);
    getAttributeValue(line, CAMERA_POS_Z, state->cameraFocalPos.z);
    getAttributeValue(line, CAMERA_ROT, state->cameraRotation);
    getAttributeValue(line, CAMERA_ZOOM, state->cameraZoomFactor);

    if (lineStartsWith(line, TOWER_LIST_START))
      readTowerList(infile, state->towers);
    }
  return true;
  }

bool writeHeightMap(std::ofstream& file, const HeightMap* heightMap)
  {
  const uint width = heightMap->width;
  if (heightMap->heights.size() < width * width)
    return false;

  for (uint row = 0; row < width; ++row)
    {
    for (uint col = 0; col < width; ++col)
      file << heightMap->getHeight(col, width - row - 1) << " ";
    file << "\n";
    }
  return true;
  }

void writeTowers(std::ofstream& file, const std::vector<TowerState>* towerStates)
  {
  file << "\n";
  for (const TowerState& state : *towerStates)
    {
    file << TOWER_START << "\n";
    file << TOWER_TYPE << " " << state.type << "\n";
    file << TOWER_POS_X << " " << state.position.x << "\n";
    file << TOWER_POS_Y << " " << state.position.y << "\n";
    file << TOWER_POS_Z << " " << state.position.z << "\n";
    file << TOWER_ROT << " " << state.rotation << "\n";
    file << TOWER_HEALTH << " " << state.health << "\n";
    file << TOWER_ENERGY << " " << state.energy << "\n";
    file << TOWER_CONSTR << " " << (state.underConstruction ? 1 : 0) << "\n";
    file << TOWER_END << "\n";
    file << "\n";
    }
  }

bool SMGameSaveLoad::saveGame(const SMGameState* state, string filePath)
  {
  if (!state)
    return false;

  string errMsg = "Failed to save game to " + filePath + ". ";
  std::ofstream file(filePath);
  if (file)
    {
    file << SAVE_FILE_VERIFY_LINE << "\n";
    file << "v" << CURRENT_VERSION << "\n";
    std::time_t timeNow = std::time(0);
    file << std::ctime(&timeNow);
    file << "\n";

    //  camera
    file << CAMERA_POS_X << " " << state->cameraFocalPos.x << "\n";
    file << CAMERA_POS_Y << " " << state->cameraFocalPos.y << "\n";
    file << CAMERA_POS_Z << " " << state->cameraFocalPos.z << "\n";
    file << CAMERA_ZOOM << " " << state->cameraZoomFactor << "\n";
    file << CAMERA_ROT << " " << state->cameraRotation << "\n";
    file << "\n";

    //  towers
    file << TOWER_LIST_START << "\n";
    writeTowers(file, &state->towers);
    file << TOWER_LIST_END << "\n";
    return true;
    }

  mathernogl::logError(errMsg + "Couldn't open file.");
  return false;
  }

bool SMGameSaveLoad::verifyState(const SMGameState* state)
  {
  return true;
  }
