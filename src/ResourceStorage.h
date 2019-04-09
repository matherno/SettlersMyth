#pragma once

#include <map>
#include <mathernogl/Types.h>
#include <GameSystem/GameSystem.h>
#include <set>

#define DEFAULT_RES_PER_STACK   9

/*
*   
*/

struct ResourceStack
  {
  uint id = 0;
  uint amount = 0;
  };

struct ResourceLock
  {
  uint lockID = 0;
  uint resID = 0;
  uint resAmount = 0;
  ResourceLock() {}
  ResourceLock(uint lockID, uint resID, uint resAmount) : lockID(lockID), resID(resID), resAmount(resAmount) {}
  bool isValid() const { return lockID > 0; }
  void reset() { *this = ResourceLock(); }
  };

struct ResourceReserve
  {
  uint reserveID = 0;
  uint resID = 0;
  uint resAmount = 0;
  ResourceReserve() {}
  ResourceReserve(uint reserveID, uint resID, uint resAmount) : reserveID(reserveID), resID(resID), resAmount(resAmount) {}
  bool isValid() const { return reserveID > 0; }
  void reset() { *this = ResourceReserve(); }
  };

class ResourceStorage
  {
public:
  typedef std::function<void()> ResourceObserverFunc;

private:
  std::vector<ResourceStack> resourceStacks;
  mathernogl::MappedList<ResourceLock> resourceLocks;
  mathernogl::MappedList<ResourceReserve> resourceReserves;
  std::map<uint, uint> resAmountLocked;
  std::map<uint, uint> resAmountReserved;
  std::set<uint> inputResources;
  uint totalResCount = 0;
  uint maxResPerStack = 9;
  uint nextLockID = 1;
  uint nextReserveID = 1;
  ResourceObserverFunc resObserver;

public:
  ResourceStorage();

  void setResourceObserverFunc(ResourceObserverFunc func);

  const std::vector<ResourceStack>* getResourceStacks() const;
  virtual void forEachResource(std::function<void(uint id, uint amount)> func, bool includeLocked = false, bool includeInputs = false) const;
  virtual uint resourceCount(uint id, bool includeLocked = false, bool includeInputs = false) const;
  virtual uint totalResourceCount() const { return totalResCount; }
  virtual bool canStoreResource(uint id, uint amount) const;
  virtual void transferAllResourcesTo(ResourceStorage* receiver);
  virtual bool takeResource(uint id, uint amount, bool canTakeInput = false);
  virtual bool storeResource(uint id, uint amount);
  virtual uint takeAllResource(uint id, uint maxAmount = 99999);
  virtual void clearAllResources();

  virtual void setupStackCount(uint stackCount, uint maxResPerStack = DEFAULT_RES_PER_STACK);
  virtual uint getStackCount() const { return resourceStacks.size(); }
  virtual uint getMaxResPerStack() const { return maxResPerStack; }

  //  locked resources are unable to be taken, unless unlocked
  virtual ResourceLock lockResource(uint id, uint amount);
  virtual void unlockResource(const ResourceLock& lock);
  virtual bool takeLockedResource(const ResourceLock& lock);
  virtual uint lockedResourceCount(uint id) const;

  //  reserved resource space is space in the resource stacks that has been reserved, and can't be filled unless freed
  virtual ResourceReserve reserveResourceSpace(uint id, uint amount);
  virtual void freeResourceSpace(const ResourceReserve& reserve);
  virtual bool fillResourceSpace(const ResourceReserve& reserve);
  virtual uint reservedResourceSpaceCount(uint id) const;

  //  input resources are resources that are required by this resource storage, and can't be taken
  virtual void registerResourceAsInput(uint id);
  };
