#pragma once

#include <mathernogl/Types.h>
#include <memory>

/*
*   
*/

struct SMActorCommand
  {
  uint id = 0;
  std::shared_ptr<void> extra = nullptr;
  SMActorCommand(){}
  SMActorCommand(uint id, std::shared_ptr<void> extra = nullptr) : id (id), extra(extra) {}
  };


#define CMD_NONE                  0
#define CMD_IDLE                  1
#define CMD_HARVEST               100
#define CMD_MANUFACTURE           101
#define CMD_COLLECT               102
#define CMD_RETURN_TO_BASE        103
