#pragma once
//
// Created by matt on 28/12/18.
//


#include <TowOff/RenderSystem/RenderSystem.h>


#define VOXEL_SIZE 0.125
#define DYNAMIC_VOXEL_SIZE 0.0625
#define SEND_SETTLER_COOLDOWN 4000
#define MANUFACTURE_COOLDOWN  5000


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

