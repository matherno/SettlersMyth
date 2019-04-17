//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentUnitRecruiter.h"


/*
*   ComponentUnitRecruiterBlueprint
*/

bool ComponentUnitRecruiterBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  maxUnits = xmlComponent->IntAttribute(OD_MAXUNITS, 10);
  unitBlueprintName = xmlGetStringAttribute(xmlComponent, OD_UNIT);
  return true;
  }

bool ComponentUnitRecruiterBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  const SMGameActorBlueprint* unitBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(unitBlueprintName);
  if (!unitBlueprint)
    {
    *errorMsg = "Unit name '" + unitBlueprintName + "' unknown.";
    return false;
    }
  unitBlueprintID = unitBlueprint->id;
  return true;
  }

SMComponentPtr ComponentUnitRecruiterBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentUnitRecruiter(actor, type, this));
  }




/*
*   ComponentUnitRecruiter
*/

ComponentUnitRecruiter::ComponentUnitRecruiter(const SMGameActorPtr& actor, SMComponentType type, const ComponentUnitRecruiterBlueprint* blueprint) : BuildingComponent(actor, type), blueprint(blueprint)
  {}

void ComponentUnitRecruiter::initialise(GameContext* gameContext)
  {
  recruitTimer.setTimeOut(0);   // make recruitUnit() get called at next update
  recruitTimer.reset();
  recruitTimer.setTimeOut(RECRUIT_UNIT_COOLDOWN);
  }

void ComponentUnitRecruiter::initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent)
  {
  recruitTimer.setTimeOut(RECRUIT_UNIT_COOLDOWN);
  recruitTimer.reset();
  }

void ComponentUnitRecruiter::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  BuildingComponent::onMessage(gameContext, message, extra);

  switch (message)
    {
    case SMMessage::constructionFinished:
      recruitUnit(gameContext);
      recruitTimer.reset();
      break;
    }
  }

void ComponentUnitRecruiter::update(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  if (!building->getIsUnderConstruction())
    {
    if (recruitTimer.incrementTimer(gameContext->getDeltaTime()))
      {
      recruitUnit(gameContext);
      recruitTimer.reset();
      }
    }
  }

void ComponentUnitRecruiter::cleanUp(GameContext* gameContext)
  {
  
  }

void ComponentUnitRecruiter::recruitUnit(GameContext* gameContext)
  {
  GameActorBuilding* thisBuilding = getBuildingActor();
  if (thisBuilding->getAttachedUnits()->count() >= blueprint->maxUnits)
    return;

  SMGameContext* smGameContext = SMGameContext::cast(gameContext);
  SMGameActorPtr residentsActor = smGameContext->getGridMapHandler()->findClosestGriddedActor(gameContext, thisBuilding->getEntryPosition(), 
    [&](SMGameActorPtr actor)
    {
    if (GameActorBuilding* building = GameActorBuilding::cast(actor.get()))
      return building->getDettachedResidentUnit(blueprint->unitBlueprintID) != nullptr;
    return false;
    });

  if (GameActorBuilding* residentsBuilding = GameActorBuilding::cast(residentsActor.get()))
    {
    GameActorUnitPtr unit = residentsBuilding->getDettachedResidentUnit();
    ASSERT(unit, "");
    thisBuilding->attachUnit(unit);
    unit->returnToAttachedBuilding(gameContext);
    }
  }
