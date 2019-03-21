//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentDestroyWhenNoResource.h"


/*
*   ComponentDestroyWhenNoResourceBlueprint
*/

bool ComponentDestroyWhenNoResourceBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  return true;
  }


SMComponentPtr ComponentDestroyWhenNoResourceBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentDestroyWhenNoResource(actor, type));
  }



/*
*   ComponentDestroyWhenNoResource
*/

ComponentDestroyWhenNoResource::ComponentDestroyWhenNoResource(const SMGameActorPtr& actor, SMComponentType type) 
      : SMComponent(actor, type)
  {}

void ComponentDestroyWhenNoResource::initialise(GameContext* gameContext)
  {
  updateTimer.setTimeOut(500);
  updateTimer.reset();
  }

void ComponentDestroyWhenNoResource::update(GameContext* gameContext)
  {
  if (updateTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    if (getActor()->totalResourceCount() == 0)
      SMGameContext::cast(gameContext)->destroySMActor(getActor()->getID());
    updateTimer.reset();
    }
  }
