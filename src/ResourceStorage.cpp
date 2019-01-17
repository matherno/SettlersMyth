//
// Created by matt on 1/12/18.
//

#include <set>
#include "ResourceStorage.h"



ResourceStorage::ResourceStorage()
  {
  setupStackCount(1);
  }

const std::vector<ResourceStack>* ResourceStorage::getResourceStacks() const
  {
  return &resourceStacks;
  }

uint ResourceStorage::resourceCount(uint id, bool includeLocked) const
  {
  if (id == 0)
    return 0;
  uint count = 0;
  for (const ResourceStack& stack : resourceStacks)
    {
    if (stack.id == id)
      count += stack.amount;
    }

  const uint amountLocked = includeLocked ? 0 : lockedResourceCount(id);
  if (count <= amountLocked)
    return 0;
  return count - amountLocked;
  }

bool ResourceStorage::takeResource(uint id, uint amount)
  {
  if (resourceCount(id) >= amount)
    {
    totalResCount -= amount;

    for (auto iter = resourceStacks.rbegin(); iter != resourceStacks.rend(); ++iter)
      {
      if (iter->id == id)
        {
        const uint amountToTake = std::min(iter->amount, amount);
        iter->amount -= amountToTake;
        amount -= amountToTake;
        }
      }
    return true;
    }
  return false;
  }

bool ResourceStorage::storeResource(uint id, uint amount)
  {
  if (!canStoreResource(id, amount))
    return false;

  totalResCount += amount;

  //  store resource in stacks that already contain this resource first
  for (ResourceStack& stack : resourceStacks)
    {
    if (stack.id == id && stack.amount > 0)
      {
      const uint amountToStore = std::min(amount, maxResPerStack - stack.amount);
      stack.amount += amountToStore;
      amount -= amountToStore;
      stack.id = id;
      }
    }

  //  then store resource in empty stacks
  for (ResourceStack& stack : resourceStacks)
    {
    if (stack.id == 0 || stack.amount == 0)
      {
      const uint amountToStore = std::min(amount, maxResPerStack - stack.amount);
      stack.amount += amountToStore;
      amount -= amountToStore;
      stack.id = id;
      }
    }

  return true;
  }

uint ResourceStorage::takeAllResource(uint id, uint maxAmount)
  {
  if (id == 0)
    return 0;

  uint amountTaken = 0;
  for (auto iter = resourceStacks.rbegin(); iter != resourceStacks.rend(); ++iter)
    {
    if (iter->id == id)
      {
      const uint amountToTake = std::min(iter->amount, maxAmount);
      iter->amount -= amountToTake;
      maxAmount -= amountToTake;
      amountTaken += amountToTake;
      }
    }

  totalResCount -= amountTaken;
  return amountTaken;
  }

void ResourceStorage::clearAllResources()
  {
  for (ResourceStack& stack : resourceStacks)
    {
    stack.amount = 0;
    stack.id = 0;
    }
  ASSERT(resourceLocks.count() == 0, "");
  resourceLocks.clear();
  resAmountLocked.clear();
  totalResCount = 0;
  }

bool ResourceStorage::canStoreResource(uint id, uint amount) const
  {
  if (id == 0)
    return false;

  uint totalSpaceFree = 0;
  for (const ResourceStack& stack : resourceStacks)
    {
    if (stack.id == id || stack.id == 0 || stack.amount == 0)
      totalSpaceFree += maxResPerStack - stack.amount;
    }

  totalSpaceFree -= reservedResourceSpaceCount(id);

  return totalSpaceFree >= amount;
  }

void ResourceStorage::transferAllResourcesTo(ResourceStorage* receiver)
  {
  ASSERT(resourceLocks.count() == 0, "");
  resourceLocks.clear();
  resAmountLocked.clear();

  for (ResourceStack& stack : resourceStacks)
    {
    if (stack.id > 0 && stack.amount > 0)
      {
      receiver->storeResource(stack.id, stack.amount);
      totalResCount -= stack.amount;
      stack.amount = 0;
      }
    }
  }

void ResourceStorage::setupStackCount(uint stackCount, uint maxResPerStack)
  {
  this->maxResPerStack = (uint) mathernogl::clampi(maxResPerStack, 1, 999);
  stackCount = (uint) mathernogl::clampi(stackCount, 1, 50);
  resourceStacks.resize(stackCount);
  for (ResourceStack& stack : resourceStacks)
    stack.amount = std::min(stack.amount, maxResPerStack);
  }

void ResourceStorage::forEachResource(std::function<void(uint id, uint amount)> func, bool includeLocked) const
  {
  std::set<uint> resourceAmounts;
  for (const ResourceStack& stack : resourceStacks)
    {
    if (stack.id > 0 && stack.amount > 0)
      resourceAmounts.insert(stack.id);
    }

  for (uint id : resourceAmounts)
    func(id, resourceCount(id, includeLocked));
  }

ResourceLock ResourceStorage::lockResource(uint id, uint amount)
  {
  if (resourceCount(id) >= amount)
    {
    if (resAmountLocked.count(id) == 0)
      resAmountLocked[id] = amount;
    else
      resAmountLocked[id] += amount;

    ResourceLock lock(nextLockID++, id, amount);
    resourceLocks.add(lock, lock.lockID);
    return lock;
    }
  return ResourceLock();
  }

void ResourceStorage::unlockResource(const ResourceLock& lock)
  {
  if(lock.isValid() && resourceLocks.contains(lock.lockID))
    {
    resourceLocks.remove(lock.lockID);
    if (resAmountLocked.count(lock.resID) > 0 && resAmountLocked[lock.resID] >= lock.resAmount)
      resAmountLocked[lock.resID] -= lock.resAmount;
    else
      resAmountLocked[lock.resID] = 0;
    }
  }

bool ResourceStorage::takeLockedResource(const ResourceLock& lock)
  {
  if(lock.isValid() && resourceLocks.contains(lock.lockID))
    {
    unlockResource(lock);
    return takeResource(lock.resID, lock.resAmount);
    }
  return false;
  }

uint ResourceStorage::lockedResourceCount(uint id) const
  {
  if(resAmountLocked.count(id) == 0)
    return 0;
  return resAmountLocked.at(id);
  }

ResourceReserve ResourceStorage::reserveResourceSpace(uint id, uint amount)
  {
  if (canStoreResource(id, amount))
    {
    if (resAmountReserved.count(id) == 0)
      resAmountReserved[id] = amount;
    else
      resAmountReserved[id] += amount;

    ResourceReserve reserveHandle(nextReserveID++, id, amount);
    resourceReserves.add(reserveHandle, reserveHandle.reserveID);
    return reserveHandle;
    }
  return ResourceReserve();
  }

void ResourceStorage::freeResourceSpace(const ResourceReserve& reserve)
  {
  if(reserve.isValid() && resourceReserves.contains(reserve.reserveID))
    {
    resourceReserves.remove(reserve.reserveID);
    if (resAmountReserved.count(reserve.resID) > 0 && resAmountReserved[reserve.resID] >= reserve.resAmount)
      resAmountReserved[reserve.resID] -= reserve.resAmount;
    else
      resAmountReserved[reserve.resID] = 0;
    }
  }

bool ResourceStorage::fillResourceSpace(const ResourceReserve& reserve)
  {
  if(reserve.isValid() && resourceReserves.contains(reserve.reserveID))
    {
    freeResourceSpace(reserve);
    return storeResource(reserve.resID, reserve.resAmount);
    }
  return false;
  }

uint ResourceStorage::reservedResourceSpaceCount(uint id) const
  {
  if(resAmountReserved.count(id) == 0)
    return 0;
  return resAmountReserved.at(id);
  }
