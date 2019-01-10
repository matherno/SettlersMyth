#pragma once

#include <mathernogl/Types.h>
#include <memory>

/*
*   
*/

struct SMActorCommand
  {
  uint id = 0;
  int extra = 0;
  SMActorCommand(){}
  SMActorCommand(uint id, int extra = 0) : id (id), extra(extra) {}
  };


#define CMD_NONE                  0
#define CMD_IDLE                  1
#define CMD_HARVEST               100
#define CMD_MANUFACTURE           101
#define CMD_COLLECT               102
#define CMD_RETURN_TO_BASE        103
#define CMD_MOVE_TO_MANUF_SPOT    104
#define CMD_COLLECT_NOT_STORAGE   105
