//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include <Resources.h>
#include "ComponentConstructor.h"


/*
*   ComponentConstructorBlueprint
*/

bool ComponentConstructorBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  constructionPackID = 0;
  constructionPackName = xmlGetStringAttribute(xmlComponent, OD_CONSTRUCTIONPACK);
  constructionTime = xmlComponent->IntAttribute(OD_CONSTRUCTIONTIME, 5000);
  return true;
  }

bool ComponentConstructorBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  const SMGameActorBlueprint* resourceBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(constructionPackName);
  if (!resourceBlueprint)
    {
    *errorMsg = "Construction pack name '" + constructionPackName + "' unknown.";
    return false;
    }
  constructionPackID = resourceBlueprint->id;
  return true;
  }

SMComponentPtr ComponentConstructorBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentConstructor(actor, type, this));
  }



/*
*   ConstructorUnitCommand
*/

ConstructorUnitCommand::ConstructorUnitCommand(uint underConstructBuildingID, int constructTime) : underConstructBuildingID(underConstructBuildingID), constructTime(constructTime)
  {
  
  }

void ConstructorUnitCommand::onStart(GameContext* gameContext, GameActorUnit* unit)
  {
  GameActorBuilding* underConstructBuilding = getUnderConstructionBuilding(gameContext);
  GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
  if (!underConstructBuilding || !attachedBuilding)
    {
    cancelCommand(gameContext, unit);
    return; 
    }

  const uint constructionPackID = underConstructBuilding->getConstructionPackID();
  buildingResReserve = underConstructBuilding->getConstructionResourceStorage()->reserveResourceSpace(constructionPackID, 1);
  if (!buildingResReserve.isValid())
    {
    ASSERT(false, "Resource reserving wasn't valid?");
    cancelCommand(gameContext, unit);
    return;
    }

  if (!attachedBuilding->takeResource(constructionPackID, 1, true))
    {
    cancelCommand(gameContext, unit);
    return;
    }

  state = MOVING_TO_BUILDING;
  unit->dropAllResources(gameContext);
  unit->storeResource(constructionPackID, 1);
  setTargetPosition(underConstructBuilding->getEntryPosition(), 180);
  }

void ConstructorUnitCommand::onUpdate(GameContext* gameContext, GameActorUnit* unit)
  {
  if (state == CONSTRUCTING)
    {
    if (constructionTimer.incrementTimer(gameContext->getDeltaTime()))
      {
      GameActorBuilding* underConstructBuilding = getUnderConstructionBuilding(gameContext);
      if (underConstructBuilding)
        {
        underConstructBuilding->getConstructionResourceStorage()->fillResourceSpace(buildingResReserve);
        buildingResReserve.reset();
        }
      endCommand(gameContext, unit);
      }
    }
  }
  
void ConstructorUnitCommand::onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled)
  {
  if (wasCancelled)
    {
    GameActorBuilding* underConstructBuilding = getUnderConstructionBuilding(gameContext);
    if (underConstructBuilding && buildingResReserve.isValid())
      underConstructBuilding->getConstructionResourceStorage()->freeResourceSpace(buildingResReserve);
    }

  unit->returnToAttachedBuilding(gameContext, buildingResReserve);
  }
  
void ConstructorUnitCommand::onReachedTarget(GameContext* gameContext, GameActorUnit* unit)
  {
  if (state == MOVING_TO_BUILDING)
    {
    GameActorBuilding* underConstructBuilding = getUnderConstructionBuilding(gameContext);
    if (!underConstructBuilding)
      {
      cancelCommand(gameContext, unit);
      return;
      }
    unit->clearAllResources();
    state = CONSTRUCTING;
    constructionTimer.setTimeOut(constructTime);
    constructionTimer.reset();
    }
  }
  
GameActorBuilding* ConstructorUnitCommand::getUnderConstructionBuilding(GameContext* gameContext) const
  {
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  return GameActorBuilding::cast(smGameContext->getSMGameActor(underConstructBuildingID).get());
  }




/*
*   ComponentConstructor
*/

ComponentConstructor::ComponentConstructor(const SMGameActorPtr& actor, SMComponentType type, const ComponentConstructorBlueprint* blueprint) : BuildingComponent(actor, type), blueprint(blueprint)
  {}

void ComponentConstructor::initialise(GameContext* gameContext)
  {
  sendUnitTimer.setTimeOut(SEND_SETTLER_COOLDOWN);
  sendUnitTimer.reset();
  }

void ComponentConstructor::update(GameContext* gameContext)
  {
  if (sendUnitTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    sendUnitToConstruct(gameContext);
    sendUnitTimer.reset();
    }
  }

void ComponentConstructor::cleanUp(GameContext* gameContext)
  {
  
  }

void ComponentConstructor::sendUnitToConstruct(GameContext* gameContext)
  {
  GameActorBuilding* thisBuilding = getBuildingActor();
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  if (thisBuilding->resourceCount(blueprint->constructionPackID, false, true) == 0)
    return;

  //  get an idle attached unit
  GameActorUnit* unit = thisBuilding->getIdleUnit().get();
  if (!unit)
    return;

  //  find an under-construction building that requires the kind of construction packs that this constructor uses
  SMGameActorPtr buildingActorToConstruct = smGameContext->getGridMapHandler()->findClosestGriddedActor(gameContext, thisBuilding->getEntryPosition(), 
    [&](SMGameActorPtr actor)
    {
    if (actor->getID() == thisBuilding->getID())
      return false;
    GameActorBuilding* buildingActor = GameActorBuilding::cast(actor.get());
    if (buildingActor && buildingActor->getIsUnderConstruction() && buildingActor->getConstructionPackID() == blueprint->constructionPackID)
      return buildingActor->getConstructionResourceStorage()->canStoreResource(blueprint->constructionPackID, 1);
    return false;
    });

  // then tell the idle unit to take a construction pack to this building and construct it a bit
  if (buildingActorToConstruct)
    {
    UnitCommandPtr command(new ConstructorUnitCommand(buildingActorToConstruct->getID(), blueprint->constructionTime));
    unit->performCommand(command);
    }
  }
