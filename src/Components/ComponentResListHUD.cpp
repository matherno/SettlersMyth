//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include <Resources.h>
#include "ComponentResListHUD.h"
#include "UIResourceAmount.h"


/*
*   ComponentResListHUDBlueprint
*/

bool ComponentResListHUDBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  XMLElement* xmlResources = xmlComponent->FirstChildElement(OD_RESOURCE);
  while(xmlResources)
    {
    resourcesByName.push_back(xmlGetStringAttribute(xmlResources, OD_NAME));
    xmlResources = xmlResources->NextSiblingElement(OD_RESOURCE);
    }

  return true;
  }

bool ComponentResListHUDBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  //  verify input resource names and get their corresponding blueprint ids
  for (string resName : resourcesByName)
    {
    const SMGameActorBlueprint* resourceBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(resName);
    if (!resourceBlueprint)
      {
      *errorMsg = "Resource name '" + resName + "' unknown.";
      return false;
      }
    resources.push_back(resourceBlueprint->id);
    }

  return true;
  }

SMComponentPtr ComponentResListHUDBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentResListHUD(actor, type, this));
  }




ComponentResListHUD::ComponentResListHUD(const SMGameActorPtr& actor, SMComponentType type, const ComponentResListHUDBlueprint* blueprint) 
  : SMComponent(actor, type), blueprint(blueprint)
  {}

void ComponentResListHUD::initialise(GameContext* gameContext)
  {

  }

void ComponentResListHUD::update(GameContext* gameContext)
  {

  }

void ComponentResListHUD::cleanUp(GameContext* gameContext)
  {
  
  }

int ComponentResListHUD::onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset)
  {
  const int columnCount = mathernogl::clampi(blueprint->resources.size(), 1, 3);
  uiResourceList.reset(new UIResourceAmountList(gameContext->getUIManager()->getNextComponentID(), getActor().get(), columnCount));
  for (uint resID : blueprint->resources)
    uiResourceList->addResource(resID, gameContext);
  uiResourceList->setOffset(Vector2D(0, yOffset));
  uiResourceList->setHorizontalAlignment(Alignment::alignmentCentre);
  parentPanel->addChild(uiResourceList);
  return uiResourceList->getPixelHeightUsed();
  }

void ComponentResListHUD::onUpdateSelectionHUD(GameContext* gameContext) 
  {
  uiResourceList->updateResources();
  }