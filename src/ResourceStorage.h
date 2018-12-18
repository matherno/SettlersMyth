#pragma once

#include <map>
#include <mathernogl/Types.h>
#include <TowOff/GameSystem/GameSystem.h>

/*
*   
*/


class ResourceStorage
  {
private:
  std::map<uint, int> storedResources;
  int maxCapacity = -1;                 //  -1 for infinite capacity
  bool capacityPerResource = false;     //  if true, then maxCapacity applies to each resource, rather than max total resources
  int totalResCount = 0;

public:
  const std::map<uint, int>* getStoredResources() const;
  virtual int resourceCount(uint id) const;
  virtual int totalResourceCount() const { return totalResCount; }
  virtual int getMaxCapacity() const { return maxCapacity; }
  virtual bool isCapacityPerResource() const { return capacityPerResource; }
  virtual bool canStoreResource(uint id, int amount) const;
  virtual void transferAllResourcesTo(ResourceStorage* receiver);

  virtual bool takeResource(uint id, int amount);
  virtual bool storeResource(uint id, int amount);
  virtual int takeAllResource(uint id, int maxAmount = 99999);
  virtual void clearAllResources();
  virtual void setMaxCapacity(int capacity, bool perResource) { maxCapacity = capacity; capacityPerResource = perResource; }
  };
