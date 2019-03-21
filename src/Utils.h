#pragma once
//
// Created by matt on 28/12/18.
//


#include <TowOff/RenderSystem/RenderSystem.h>


#define GAME_TITLE "Void Populi"


#define VOXEL_SIZE 0.125
#define DYNAMIC_VOXEL_SIZE 0.0625
#define SEND_SETTLER_COOLDOWN 4000
#define MANUFACTURE_COOLDOWN  5000

#define HUD_COL_BG              colToVec3(62, 62, 63)
#define HUD_COL_BORDER          colToVec3(30, 30, 30)
#define HUD_BORDER_SIZE         3
#define BTN_PRESSED_COL         Vector3D(0.5, 0.5, 0.6)
#define BTN_UNPRESSED_COL       Vector3D(0.15)
#define BTN_BORDER_SIZE         2
#define MENU_BTN_BG_COL         Vector3D(0.05)
#define MENU_BTN_TEXT_COL       Vector3D(0.3)
#define MENU_BTN_PRESS_COL      Vector3D(0.2, 0.3, 0.5)
#define MENU_BTN_UNPRESS_COL    Vector3D(0.3)
#define MENU_BTN_BORDER_SIZE    3


static Vector3D colToVec3(double r, double g, double b, bool gammaCorrect = false)
  {
  Vector3D vector(r, g, b);
  vector /= 255;
  if (gammaCorrect)
    {
    vector.x = std::pow(vector.x, 2.2);
    vector.y = std::pow(vector.y, 2.2);
    vector.z = std::pow(vector.z, 2.2);
    }
  return vector;
  }


enum class SMMessage
  {
  actorPositionChanged,
  actorRotationChanged,
  attachedUnitCmdEnded,       //  extra is pointer to the UnitCommand that was ended
  attachedUnitCmdCancelled,   //  extra is pointer to the UnitCommand that was cancelled
  };


enum class SMGameActorType
  {
  unknown,
  building,
  unit,
  deposit,
  obstacle,
  resource,
  };

enum class SMComponentType
  {
  unknown,
  voxelModelRenderable,
  buildingHarvester,
  buildingManufacturer,
  buildingStorage,
  resourceStacks,
  resourcePickup,
  destroyWhenNoResource,
  };


namespace SMTypes
  {
  static const std::map<SMComponentType, string> componentNames
    {
      { SMComponentType::voxelModelRenderable, "VoxelRenderable" },
      { SMComponentType::buildingHarvester, "BuildingHarvester" },
      { SMComponentType::buildingManufacturer, "BuildingManufacturer" },
      { SMComponentType::buildingStorage, "BuildingStorage" },
      { SMComponentType::resourceStacks, "ResourceStacks" },
      { SMComponentType::resourcePickup, "ResourcePickup" },
      { SMComponentType::destroyWhenNoResource, "DestroyWhenNoResource" },
    };

  static const std::map<SMGameActorType, string> typeNames
    {
      { SMGameActorType::building, "Building" },
      { SMGameActorType::unit, "Unit" },
      { SMGameActorType::deposit, "Deposit" },
      { SMGameActorType::obstacle, "Obstacle" },
      { SMGameActorType::resource, "Resource" },
    };

  static string getComponentName(SMComponentType type)
    {
    if (componentNames.count(type) > 0)
      return componentNames.at(type);
    ASSERT(false, "");
    return "";
    }

  static SMComponentType getComponentFromName(string name)
    {
    for (auto pair : componentNames)
      {
      if (pair.second.compare(name) == 0)
        return pair.first;
      }
    return SMComponentType::unknown;
    }

  static string getGameActorTypeName(SMGameActorType type)
    {
    if (typeNames.count(type) > 0)
      return typeNames.at(type);
    ASSERT(false, "");
    return "";
    }

  static SMGameActorType getGameActorTypeFromName(string name)
    {
    for (auto pair : typeNames)
      {
      if (pair.second.compare(name) == 0)
        return pair.first;
      }
    return SMGameActorType::unknown;
    }
  }