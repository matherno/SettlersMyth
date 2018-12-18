//
// Created by matt on 1/12/18.
//

#include "ResourceStorage.h"

const std::map<uint, int>* ResourceStorage::getStoredResources() const
  {
  return &storedResources;
  }

int ResourceStorage::resourceCount(uint id) const
  {
  if (storedResources.count(id) > 0)
    return storedResources.at(id);
  return 0;
  }

bool ResourceStorage::takeResource(uint id, int amount)
  {
  if (resourceCount(id) >= amount)
    {
    storedResources[id] -= amount;
    totalResCount -= amount;
    return true;
    }
  return false;
  }

bool ResourceStorage::storeResource(uint id, int amount)
  {
  if (!canStoreResource(id, amount))
    return false;
  storedResources[id] = resourceCount(id) + amount;
  totalResCount += amount;
  return true;
  }

int ResourceStorage::takeAllResource(uint id, int maxAmount)
  {
  int amountToTake = 0;
  if (storedResources.count(id) > 0)
    {
    amountToTake = std::min(maxAmount, resourceCount(id));
    storedResources[id] -= amountToTake;
    totalResCount -= amountToTake;
    }
  return amountToTake;
  }

void ResourceStorage::clearAllResources()
  {
  storedResources.clear();
  totalResCount = 0;
  }

bool ResourceStorage::canStoreResource(uint id, int amount) const
  {
  if (maxCapacity < 0)
    return true;
  if (!capacityPerResource && totalResCount + amount > maxCapacity)
    return false;
  if (capacityPerResource && resourceCount(id) + amount > maxCapacity)
    return false;
  return true;
  }

void ResourceStorage::transferAllResourcesTo(ResourceStorage* receiver)
  {
  for (auto pair : storedResources)
    {
    if (pair.first)
      receiver->storeResource(pair.first, pair.second);
    takeAllResource(pair.first);
    }
  }
