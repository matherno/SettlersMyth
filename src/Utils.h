#pragma once
//
// Created by matt on 28/12/18.
//


#include <TowOff/RenderSystem/RenderSystem.h>


#define VOXEL_SIZE 0.125
#define DYNAMIC_VOXEL_SIZE 0.0625
#define SEND_SETTLER_COOLDOWN 4000
#define MANUFACTURE_COOLDOWN  5000

#define HUD_COL_BG        colToVec3(62, 62, 63)
#define HUD_COL_BORDER    colToVec3(30, 30, 30)
#define HUD_BORDER_SIZE   3
#define BTN_PRESSED_COL   Vector3D(0.5, 0.5, 0.6)
#define BTN_UNPRESSED_COL Vector3D(0.15)
#define BTN_BORDER_SIZE   2

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

