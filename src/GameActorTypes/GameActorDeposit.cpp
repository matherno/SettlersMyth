//
// Created by matt on 27/02/19.
//

#include "GameActorDeposit.h"
#include "BlueprintFileHelper.h"
#include "SMGameContext.h"


bool GameActorDepositBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  if(!SMGameActorBlueprint::loadFromXML(xmlComponent, errorMsg))
    return false;

  auto xmlDeposit = xmlComponent->FirstChildElement(OD_DEPOSIT);
  if (xmlDeposit)
    {
    resourceName = xmlGetStringAttribute(xmlDeposit, OD_RESOURCENAME);
    xmlDeposit->QueryAttribute(OD_RESOURCEAMOUNT, &resourceAmount);
    }
  else
    {
    *errorMsg = "No deposit definition found";
    return false;
    }

  return true;
  }

bool GameActorDepositBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  const SMGameActorBlueprint* resourceBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(resourceName);
  if (!resourceBlueprint)
    {
    *errorMsg = "Resource name '" + resourceName + "' unknown.";
    return false;
    }

  resourceBlueprintID = resourceBlueprint->id;

  return SMGameActorBlueprint::finaliseLoading(gameContext, errorMsg);
  }

SMGameActor* GameActorDepositBlueprint::doConstructActor(uint actorID) const
  {
  GameActorDeposit* actor = new GameActorDeposit(actorID, id);
  actor->resourceBlueprintID = resourceBlueprintID;
  actor->setupStackCount(1, resourceAmount);
  actor->storeResource(resourceBlueprintID, resourceAmount);
  return actor;
  }





GameActorDeposit::GameActorDeposit(uint id, uint typeID) : SMGameActor(id, typeID)
  {
  }

int GameActorDeposit::depositResourceCount() const
  {
  return resourceCount(getDepositResourceBlueprintID());
  }
  
int GameActorDeposit::getDepositResourceBlueprintID() const
  {
  return resourceBlueprintID;
  }