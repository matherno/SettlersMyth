//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentInitResources.h"


/*
*   ComponentInitResourcesBlueprint
*/

bool ComponentInitResourcesBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  XMLElement* xmlResource = xmlComponent->FirstChildElement(OD_RESOURCE);
  while (xmlResource)
    {
    string resName = xmlGetStringAttribute(xmlResource, OD_NAME);
    int resAmount = xmlResource->IntAttribute(OD_AMOUNT, 0);
    initialResourcesByName[resName] = resAmount;
    xmlResource = xmlResource->NextSiblingElement(OD_RESOURCE);
    }
  return true;
  }


SMComponentPtr ComponentInitResourcesBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentInitResources(actor, type, this));
  }

bool ComponentInitResourcesBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  for (auto pair : initialResourcesByName)
    {
    const SMGameActorBlueprint* resourceBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(pair.first);
    if (!resourceBlueprint)
      {
      *errorMsg = "Initial resource name '" + pair.first + "' unknown.";
      return false;
      }
    initialResources[resourceBlueprint->id] = pair.second;
    }
  return true;
  }



/*
*   ComponentInitResources
*/

ComponentInitResources::ComponentInitResources(const SMGameActorPtr& actor, SMComponentType type, const ComponentInitResourcesBlueprint* blueprint) 
      : SMComponent(actor, type), blueprint(blueprint)
  {}

void ComponentInitResources::initialise(GameContext* gameContext)
  {
  for (auto pair : blueprint->initialResources)
    getActor()->storeResource(pair.first, pair.second);
  }

void ComponentInitResources::initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent)
  {
  }

