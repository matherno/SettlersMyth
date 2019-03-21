//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include <Resources.h>
#include "ComponentHarvester.h"
#include "GameActorTypes/GameActorDeposit.h"
#include "GameActorTypes/GameActorResource.h"


/*
*   ComponentHarvesterBlueprint
*/

bool ComponentHarvesterBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  depositName = xmlGetStringAttribute(xmlComponent, OD_DEPOSIT);
  xmlComponent->QueryAttribute(OD_HARVESTTIME, &harvestTime);
  return true;
  }

bool ComponentHarvesterBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  const SMGameActorBlueprint* depositBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(depositName);
  if (!depositBlueprint)
    {
    *errorMsg = "Deposit name '" + depositName + "' unknown.";
    return false;
    }

  depositBlueprintID = depositBlueprint->id;

  return true;
  }

SMComponentPtr ComponentHarvesterBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentHarvester(actor, type, this));
  }




/*
*   HarvestUnitCommand
*/

static const float harvestOffsetAmt = 0.475;
static const std::vector<Vector2D> depositHarvestOffsets =
  {
  Vector2D(harvestOffsetAmt,  0),
  Vector2D(harvestOffsetAmt,  harvestOffsetAmt),
  Vector2D(harvestOffsetAmt,  -harvestOffsetAmt),
  Vector2D(-harvestOffsetAmt, 0),
  Vector2D(-harvestOffsetAmt, harvestOffsetAmt),
  Vector2D(-harvestOffsetAmt, -harvestOffsetAmt),
  Vector2D(0,                 harvestOffsetAmt),
  Vector2D(0,                 -harvestOffsetAmt),
  };


HarvestUnitCommand::HarvestUnitCommand(uint depositActorID, int harvestTime) : depositActorID(depositActorID), harvestTime(harvestTime)
  {
  
  }

void HarvestUnitCommand::onStart(GameContext* gameContext, GameActorUnit* unit)
  {
  GameActorDeposit* deposit = getResourceDeposit(gameContext);
  GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
  if (!deposit || !attachedBuilding)
    {
    cancelCommand(gameContext, unit);
    return; 
    }

  buildingResReserve = attachedBuilding->reserveResourceSpace(deposit->getDepositResourceBlueprintID(), 1);
  depositResLock = deposit->lockResource(buildingResReserve.resID, buildingResReserve.resAmount);

  if (!buildingResReserve.isValid() || !depositResLock.isValid())
    {
    ASSERT(false, "Resource lock or reserving wasn't valid?");
    cancelCommand(gameContext, unit);
    return;
    }

  state = MOVING_TO_DEPOSIT;
  const Vector2D harvestOffset = depositHarvestOffsets[mathernogl::RandomGenerator::randomInt(0, (int)depositHarvestOffsets.size() - 1)];
  const double targetRotation = -mathernogl::ccwAngleBetween(Vector2D(0, 1), harvestOffset.getUniform() * -1);
  setTargetPosition(deposit->getPosition() + harvestOffset, targetRotation);
  unit->dropAllResources(gameContext);
  }

void HarvestUnitCommand::onUpdate(GameContext* gameContext, GameActorUnit* unit)
  {
  if (state == HARVESTING_DEPOSIT)
    {
    if (harvestTimer.incrementTimer(gameContext->getDeltaTime()))
      {
      GameActorDeposit* deposit = getResourceDeposit(gameContext);
      if(deposit->takeLockedResource(depositResLock))
        unit->storeResource(depositResLock.resID, depositResLock.resAmount);
      endCommand(gameContext, unit);
      }
    }
  }
  
void HarvestUnitCommand::onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled)
  {
  if (wasCancelled)
    {
    GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
    if (attachedBuilding && buildingResReserve.isValid())
      attachedBuilding->freeResourceSpace(buildingResReserve);

    GameActorDeposit* deposit = getResourceDeposit(gameContext);
    if (deposit && depositResLock.isValid())
      deposit->unlockResource(depositResLock);
    }

  unit->returnToAttachedBuilding(gameContext, buildingResReserve);
  }
  
void HarvestUnitCommand::onReachedTarget(GameContext* gameContext, GameActorUnit* unit)
  {
  if (state == MOVING_TO_DEPOSIT)
    {
    state = HARVESTING_DEPOSIT;
    harvestTimer.setTimeOut(harvestTime);
    harvestTimer.reset();
    }
  }
  
GameActorDeposit* HarvestUnitCommand::getResourceDeposit(GameContext* gameContext) const
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  return GameActorDeposit::cast(smGameContext->getSMGameActor(depositActorID).get());
  }



/*
*   ComponentHarvester
*/

ComponentHarvester::ComponentHarvester(const SMGameActorPtr& actor, SMComponentType type, const ComponentHarvesterBlueprint* blueprint) : BuildingComponent(actor, type), blueprint(blueprint)
  {}

void ComponentHarvester::initialise(GameContext* gameContext)
  {
  sendUnitTimer.setTimeOut(SEND_SETTLER_COOLDOWN);
  sendUnitTimer.reset();

  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  const GameActorDepositBlueprint* depoBlueprint = GameActorDepositBlueprint::cast(smGameContext->getGameObjectFactory()->getGameActorBlueprint(blueprint->depositBlueprintID));
  if (depoBlueprint)
    resourceBlueprintID = depoBlueprint->resourceBlueprintID;
  }

void ComponentHarvester::update(GameContext* gameContext)
  {
  if (sendUnitTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    GameActorBuilding* building = getBuildingActor();
    SMGameContext* smGameContext = SMGameContext::cast(gameContext);

    if(building->canStoreResource(resourceBlueprintID, 1))
      {
      SMGameActorPtr deposit = smGameContext->getGridMapHandler()->findClosestGriddedActor(gameContext, building->getEntryPosition(), 
        [&](SMGameActorPtr actor)
        {
        if (actor->getBlueprintTypeID() != blueprint->depositBlueprintID)
          return false;
        GameActorDeposit* deposit = GameActorDeposit::cast(actor.get());
        if (deposit)
          return deposit->depositResourceCount() > 0;
        return false;
        }
      );

      GameActorUnit* unit = building->getIdleUnit().get();
      if (deposit && unit)
        {
        UnitCommandPtr command(new HarvestUnitCommand(deposit->getID(), blueprint->harvestTime));
        unit->performCommand(command);
        }
      }
    sendUnitTimer.reset();
    }
  }

void ComponentHarvester::cleanUp(GameContext* gameContext)
  {
  
  }

void ComponentHarvester::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  
  }

