//
// Created by matt on 7/01/19.
//

#include <SMGameContext.h>
#include <GameObjectDefs/StaticObjectDef.h>
#include <GameObjectDefs/ResourceDef.h>
#include "ResourceStackBehaviour.h"

#define MAX_STACK_SIZE 9u


void ResourceStackBehaviour::initialise(SMGameActor* gameActor, GameContext* gameContext)
  {
  const StaticObjectDef* staticDef = StaticObjectDef::cast(gameActor->getDef());
  if (!staticDef)
    return;
  for (GridXY pos : staticDef->resourceStackSpots)
    resourceStacks.emplace_back(pos);
  ASSERT(staticDef->resourceStackSpots.size() == resourceStacks.size(), "");
  }

void ResourceStackBehaviour::update(SMGameActor* gameActor, GameContext* gameContext)
  {
  const std::vector<ResourceStack>* storedStacks = gameActor->getResourceStacks();

  int stackIdx = 0;
  for (const ResourceStack& stack : *storedStacks)
    {
    const uint storedResID = stack.id;
    const uint storedResAmount = (uint)std::min(stack.amount, MAX_STACK_SIZE);

    if (resourceStacks[stackIdx].id != storedResID || resourceStacks[stackIdx].amount != storedResAmount)
      {
      //  update stack and recreate actors
      resourceStacks[stackIdx].id = storedResID;
      resourceStacks[stackIdx].amount = storedResAmount;
      refreshResourceStackActors(gameActor, gameContext, (uint) stackIdx);
      }
    stackIdx++;
    }
  }

void ResourceStackBehaviour::cleanUp(SMGameActor* gameActor, GameContext* gameContext)
  {
  for (uint i = 0; i < resourceStacks.size(); ++i)
    {
    Stack& stack = resourceStacks[i];
    stack.amount = 0;
    refreshResourceStackActors(gameActor, gameContext, i);
    }
  resourceStacks.clear();
  }


static Vector3D getResourcePosInStack(const ResourceDef* resourceDef, int resNum)
  {
  if (resNum < 0 || resNum >= resourceDef->stackPosAndRots.size())
    return Vector3D(0.5, 0, -0.5);
  return resourceDef->stackPosAndRots[resNum].first;
  }

static double getResourceRotInStack(const ResourceDef* resourceDef, int resNum)
  {
  if (resNum < 0 || resNum >= resourceDef->stackPosAndRots.size())
    return 0;
  return resourceDef->stackPosAndRots[resNum].second;
  }

void ResourceStackBehaviour::refreshResourceStackActors(SMGameActor* gameActor, GameContext* gameContext, uint stackIdx)
  {
  Stack& stack = resourceStacks[stackIdx];
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  //  cleanup existing actors
  for (auto actor : stack.resourceActors)
    smGameContext->destroySMActor(actor->getID());
  stack.resourceActors.clear();

  const ResourceDef* resourceDef = ResourceDef::cast(smGameContext->getGameObjectFactory()->getGameObjectDef(stack.id).get());
  if (!resourceDef)
    return;

  //  add new actor for each resource
  for (int i = 0; i < stack.amount; ++i)
    {
    Vector3D position = getResourcePosInStack(resourceDef, i);
    Vector2D position2D = Vector2D(position.x, -position.z);
    SMGameActorPtr smActor = smGameContext->createSMGameActor(stack.id, Vector2D(gameActor->getGridPosition() + stack.pos) + position2D);
    smActor->setAllowSaving(false);
    stack.resourceActors.push_back(smActor);

    SMDynamicActor* resourceActor = SMDynamicActor::cast(smActor.get());
    ASSERT(resourceActor, "This should be a dynamic actor");
    if (resourceActor)
      {
      resourceActor->setElevation(position.y);
      resourceActor->setRotation(getResourceRotInStack(resourceDef, i));
      }
    }
  }

