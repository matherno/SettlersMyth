//
// Created by matt on 1/12/18.
//

#include "ResourceStorage.h"



ResourceStorage::ResourceStorage()
  {
  setupStackCount(1);
  }

const std::vector<ResourceStack>* ResourceStorage::getResourceStacks() const
  {
  return &resourceStacks;
  }

uint ResourceStorage::resourceCount(uint id) const
  {
  if (id == 0)
    return 0;
  uint count = 0;
  for (const ResourceStack& stack : resourceStacks)
    {
    if (stack.id == id)
      count += stack.amount;
    }
  return count;
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
  totalResCount = 0;
  }

bool ResourceStorage::canStoreResource(uint id, uint amount) const
  {
  if (id == 0)
    return false;

  for (const ResourceStack& stack : resourceStacks)
    {
    if (stack.id == id || stack.id == 0 || stack.amount == 0)
      {
      uint spaceFree = maxResPerStack - stack.amount;
      if (amount <= spaceFree)
        return true;
      amount -= spaceFree;
      }
    }
  return false;
  }

void ResourceStorage::transferAllResourcesTo(ResourceStorage* receiver)
  {
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

void ResourceStorage::forEachResource(std::function<void(uint id, uint amount)> func) const
  {
  std::map<uint, uint> resourceAmounts;
  for (const ResourceStack& stack : resourceStacks)
    {
    if (stack.id > 0 && stack.amount > 0)
      {
      if (resourceAmounts.count(stack.id) == 0)
        resourceAmounts[stack.id] = stack.amount;
      else
        resourceAmounts[stack.id] += stack.amount;
      }
    }

  for (auto& pair : resourceAmounts)
    func(pair.first, pair.second);
  }

