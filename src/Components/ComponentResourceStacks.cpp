//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentResourceStacks.h"
#include "GameActorTypes/GameActorResource.h"

#define MAX_STACK_SIZE 9u


/*
*   ComponentResourceStacksBlueprint
*/

bool ComponentResourceStacksBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  return true;
  }

bool ComponentResourceStacksBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  return true;
  }

SMComponentPtr ComponentResourceStacksBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentResourceStacks(actor, type));
  }



/*
*   ComponentResourceStacks
*/

ComponentResourceStacks::ComponentResourceStacks(const SMGameActorPtr& actor, SMComponentType type) : BuildingComponent(actor, type)
  {}

void ComponentResourceStacks::initialise(GameContext* gameContext)
  {
  resourceStacks.clear();
  const GameActorBuilding* building = getBuildingActor();
  if (building)
    {
    std::vector<GridXY> stackPositions;
    building->getResourceStackPositions(&stackPositions);
    for (GridXY pos : stackPositions)
      resourceStacks.emplace_back(pos);
    }
  }

void ComponentResourceStacks::update(GameContext* gameContext)
  {
  if (updateTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    const GameActorBuilding* building = getBuildingActor();
    const std::vector<ResourceStack>* storedStacks = building->getResourceStacks();

    int stackIdx = 0;
    for (const ResourceStack& stack : *storedStacks)
      {
      const uint storedResID = stack.id;
      const uint storedResAmount = (uint) std::min(stack.amount, MAX_STACK_SIZE);

      if (resourceStacks[stackIdx].id != storedResID || resourceStacks[stackIdx].amount != storedResAmount)
        {
        //  update stack and recreate actors
        resourceStacks[stackIdx].id = storedResID;
        resourceStacks[stackIdx].amount = storedResAmount;
        refreshResourceStackActors(gameContext, (uint) stackIdx);
        }
      stackIdx++;
      }

    updateTimer.setTimeOut(500);
    updateTimer.reset();
    }
  }

void ComponentResourceStacks::cleanUp(GameContext* gameContext)
  {
  const GameActorBuilding* building = getBuildingActor();
  for (uint i = 0; i < resourceStacks.size(); ++i)
    {
    Stack& stack = resourceStacks[i];
    stack.amount = 0;
    refreshResourceStackActors(gameContext, i);
    }
  resourceStacks.clear();
  }


static Vector3D getResourcePosInStack(const GameActorResourceBlueprint* blueprint, int resNum)
  {
  if (resNum < 0 || resNum >= blueprint->stackPosAndRots.size())
    return Vector3D(0.5, 0, -0.5);
  return blueprint->stackPosAndRots[resNum].first;
  }

static double getResourceRotInStack(const GameActorResourceBlueprint* blueprint, int resNum)
  {
  if (resNum < 0 || resNum >= blueprint->stackPosAndRots.size())
    return 0;
  return blueprint->stackPosAndRots[resNum].second;
  }

void ComponentResourceStacks::refreshResourceStackActors(GameContext* gameContext, uint stackIdx)
  {
  const GameActorBuilding* building = getBuildingActor();
  Stack& stack = resourceStacks[stackIdx];
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  //  cleanup existing actors
  for (auto actor : stack.resourceActors)
    smGameContext->destroySMActor(actor->getID());
  stack.resourceActors.clear();

  const GameActorResourceBlueprint* resourceBlueprint = GameActorResourceBlueprint::cast(smGameContext->getGameObjectFactory()->getGameActorBlueprint(stack.id));
  if (!resourceBlueprint)
    return;

  //  add new actor for each resource
  for (int i = 0; i < stack.amount; ++i)
    {
    Vector3D position = getResourcePosInStack(resourceBlueprint, i);
    Vector2D position2D = Vector2D(position.x, -position.z);
    SMGameActorPtr smActor = smGameContext->createSMGameActor(stack.id, Vector2D(stack.pos) + position2D);
    smActor->setAllowSaving(false);
    stack.resourceActors.push_back(smActor);
    smActor->setElevation(position.y);
    smActor->setRotation(getResourceRotInStack(resourceBlueprint, i));
    }
  }

