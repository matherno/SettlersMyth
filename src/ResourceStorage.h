#pragma once

#include <map>
#include <mathernogl/Types.h>
#include <TowOff/GameSystem/GameSystem.h>

#define DEFAULT_RES_PER_STACK   9

/*
*   
*/

struct ResourceStack
  {
  uint id = 0;
  uint amount = 0;
  };

class ResourceStorage
  {
private:
  std::vector<ResourceStack> resourceStacks;
  uint totalResCount = 0;
  uint maxResPerStack = 9;

public:
  ResourceStorage();

  const std::vector<ResourceStack>* getResourceStacks() const;
  virtual void forEachResource(std::function<void(uint id, uint amount)> func) const;
  virtual uint resourceCount(uint id) const;
  virtual uint totalResourceCount() const { return totalResCount; }
  virtual bool canStoreResource(uint id, uint amount) const;
  virtual void transferAllResourcesTo(ResourceStorage* receiver);
  virtual bool takeResource(uint id, uint amount);
  virtual bool storeResource(uint id, uint amount);
  virtual uint takeAllResource(uint id, uint maxAmount = 99999);
  virtual void clearAllResources();

  virtual void setupStackCount(uint stackCount, uint maxResPerStack = DEFAULT_RES_PER_STACK);
  virtual uint getStackCount() const { return resourceStacks.size(); }
  virtual uint getMaxResPerStack() const { return maxResPerStack; }
  };
