//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include <Resources.h>
#include "ComponentManufacturer.h"
#include "GameActorTypes/GameActorDeposit.h"
#include "GameActorTypes/GameActorResource.h"
#include "UIResourceAmount.h"


/*
*   ComponentManufacturerBlueprint
*/

bool ComponentManufacturerBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  xmlComponent->QueryAttribute(OD_PROCESSTIME, &manufactureTime);
  manufactureSpot = xmlGetGridXYValue(xmlComponent, OD_MANUF_SPOT);
  manufactureDir = xmlGetGridXYValue(xmlComponent, OD_MANUF_DIR);
  keepOutput = xmlComponent->BoolAttribute(OD_KEEPOUTPUT);

  //  inputs
  int inputResCount = 0;
  XMLElement* xmlInput = xmlComponent->FirstChildElement(OD_INPUT);
  while(xmlInput)
    {
    string resName = xmlGetStringAttribute(xmlInput, OD_NAME);
    uint resAmnt = 0;
    xmlInput->QueryAttribute(OD_AMOUNT, &resAmnt);
    if (resAmnt > 0 && !resName.empty())
      {
      inputsByName[resName] = resAmnt;
      inputResCount += resAmnt;
      }
    xmlInput = xmlInput->NextSiblingElement(OD_INPUT);
    }

  //  output
  XMLElement* xmlOutput = xmlComponent->FirstChildElement(OD_OUTPUT);
  if(xmlOutput)
    {
    outputResName = xmlGetStringAttribute(xmlOutput, OD_NAME);
    outputResAmount = 0;
    xmlOutput->QueryAttribute(OD_AMOUNT, &outputResAmount);
    }

  if (outputResName.empty() || outputResAmount == 0 || inputResCount == 0)
    {
    *errorMsg = "Invalid manufacturer input/output config";
    return false;
    }

  return true;
  }

bool ComponentManufacturerBlueprint::finaliseLoading(GameContext* gameContext, string* errorMsg)
  {
  //  verify input resource names and get their corresponding blueprint ids
  for (auto pair : inputsByName)
    {
    const SMGameActorBlueprint* resourceBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(pair.first);
    if (!resourceBlueprint)
      {
      *errorMsg = "Input resource name '" + pair.first + "' unknown.";
      return false;
      }
    inputs[resourceBlueprint->id] = pair.second;
    }

  //  verify output resource name and get its corresponding blueprint id
  const SMGameActorBlueprint* resourceBlueprint = SMGameContext::cast(gameContext)->getGameObjectFactory()->findGameActorBlueprint(outputResName);
  if (!resourceBlueprint)
    {
    *errorMsg = "Output resource name '" + outputResName + "' unknown.";
    return false;
    }
  outputResID = resourceBlueprint->id;

  return true;
  }

SMComponentPtr ComponentManufacturerBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentManufacturer(actor, type, this));
  }




/*
*   ManufactureUnitCommand
*/


ManufactureUnitCommand::ManufactureUnitCommand(const ComponentManufacturerBlueprint* manufactureBlueprint) : manufactureBlueprint(manufactureBlueprint)
  {
  
  }

void ManufactureUnitCommand::onStart(GameContext* gameContext, GameActorUnit* unit)
  {
  GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
  if (!attachedBuilding)
    {
    cancelCommand(gameContext, unit);
    return; 
    }

  unit->dropAllResources(gameContext);
  for (auto pair : manufactureBlueprint->inputs)
    {
    if (attachedBuilding->takeResource(pair.first, pair.second, true))
      {
      unit->storeResource(pair.first, pair.second);
      }
    else
      {
      cancelCommand(gameContext, unit);
      return;
      }
    }



  state = MOVING_TO_MANUFACTURE;
  const double targetRotation = -mathernogl::ccwAngleBetween(Vector2D(0, 1), Vector2D(manufactureBlueprint->manufactureDir).getUniform());
  setTargetPosition(attachedBuilding->getGridPosition() + manufactureBlueprint->manufactureSpot, targetRotation);
  }

void ManufactureUnitCommand::onUpdate(GameContext* gameContext, GameActorUnit* unit)
  {
  if (state == MANUFACTURING)
    {
    if (processTimer.incrementTimer(gameContext->getDeltaTime()))
      {
      unit->storeResource(outputResReserve.resID, outputResReserve.resAmount);
      endCommand(gameContext, unit);
      }
    }
  }
  
void ManufactureUnitCommand::onEnd(GameContext* gameContext, GameActorUnit* unit, bool wasCancelled)
  {
  if (wasCancelled)
    {
    GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext);
    if (attachedBuilding && outputResReserve.isValid())
      attachedBuilding->freeResourceSpace(outputResReserve);
    outputResReserve.reset();
    }

  unit->returnToAttachedBuilding(gameContext, outputResReserve);
  }
  
void ManufactureUnitCommand::onReachedTarget(GameContext* gameContext, GameActorUnit* unit)
  {
  if (state == MOVING_TO_MANUFACTURE)
    {
    GameActorBuilding* attachedBuilding = unit->getAttachedBuilding(gameContext); 
    unit->clearAllResources();     
    outputResReserve.reset();
    if (attachedBuilding)
      outputResReserve = attachedBuilding->reserveResourceSpace(manufactureBlueprint->outputResID, manufactureBlueprint->outputResAmount);

    state = MANUFACTURING;
    processTimer.setTimeOut((long)manufactureBlueprint->manufactureTime);
    processTimer.reset();
    }
  }



/*
*   ComponentManufacturer
*/

ComponentManufacturer::ComponentManufacturer(const SMGameActorPtr& actor, SMComponentType type, const ComponentManufacturerBlueprint* blueprint) : BuildingComponent(actor, type), blueprint(blueprint)
  {}

void ComponentManufacturer::initialise(GameContext* gameContext)
  {
  collectResTimer.setTimeOut(SEND_SETTLER_COOLDOWN);
  collectResTimer.reset();
  startManufactureTimer.setTimeOut(MANUFACTURE_COOLDOWN);
  startManufactureTimer.reset();
  isUnitManufacturing = false;

  resCollectIdx = 0;
  resCollectCycle.clear();
  for (auto pair : blueprint->inputs)
    {
    for (int resNum = 0; resNum < pair.second; ++resNum)
      resCollectCycle.push_back(pair.first);
    getBuildingActor()->registerResourceAsInput(pair.first);
    }
  if (blueprint->keepOutput)
    getBuildingActor()->registerResourceAsInput(blueprint->outputResID);
  std::shuffle(resCollectCycle.begin(), resCollectCycle.end(), std::default_random_engine());
  }

void ComponentManufacturer::update(GameContext* gameContext)
  {
  //  check if we should/can start a manufacture process
  if (!isUnitManufacturing && startManufactureTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    sendUnitToManufacture(gameContext);
    startManufactureTimer.reset();
    }

  //  collect resources if got space
  if (collectResTimer.incrementTimer(gameContext->getDeltaTime()))
    {
    sendUnitToCollect(gameContext);
    collectResTimer.reset();
    }
  }

void ComponentManufacturer::cleanUp(GameContext* gameContext)
  {
  
  }

void ComponentManufacturer::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  switch (message)
    {
    case SMMessage::attachedUnitCmdCancelled:
    case SMMessage::attachedUnitCmdEnded:
      if (dynamic_cast<ManufactureUnitCommand*>((UnitCommand*)extra))
        {
        ASSERT(isUnitManufacturing, "Cancelled a manufacture command, but we aren't actually manufacturing?");
        isUnitManufacturing = false;
        }
      break;
    }
  }


int ComponentManufacturer::onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset)
  {
  uiResourceListInput.reset(new UIResourceAmountList(gameContext->getUIManager()->getNextComponentID(), getActor().get()));
  for (auto pair : blueprint->inputs)
    uiResourceListInput->addResource(pair.first, gameContext);
  uiResourceListInput->setOffset(Vector2D(40, yOffset));
  uiResourceListInput->setHorizontalAlignment(Alignment::alignmentStart);
  parentPanel->addChild(uiResourceListInput);
  
  uiResourceListOutput.reset(new UIResourceAmountList(gameContext->getUIManager()->getNextComponentID(), getActor().get()));
  uiResourceListOutput->addResource(blueprint->outputResID, gameContext);
  uiResourceListOutput->setOffset(Vector2D(-40, yOffset));
  uiResourceListOutput->setHorizontalAlignment(Alignment::alignmentEnd);
  parentPanel->addChild(uiResourceListOutput);

  return uiResourceListInput->getPixelHeightUsed();
  }

void ComponentManufacturer::onUpdateSelectionHUD(GameContext* gameContext) 
  {
  uiResourceListInput->updateResources();
  uiResourceListOutput->updateResources();
  }

bool ComponentManufacturer::gotEnoughInputResources() const
  {
  const GameActorBuilding* building = getBuildingActor();
  if (building->resourceCount(blueprint->outputResID, false, true) + blueprint->outputResAmount > DEFAULT_RES_PER_STACK)
    return false;
  if(!building->canStoreResource(blueprint->outputResID, blueprint->outputResAmount))
    return false;

  for (auto pair : blueprint->inputs)
    {
    if (building->resourceCount(pair.first, false, true) < pair.second)
      return false;
    }

  return true;
  }

void ComponentManufacturer::sendUnitToCollect(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  SMGameContext* smGameContext = SMGameContext::cast(gameContext);

  //  get an idle attached unit
  GameActorUnit* unit = building->getIdleUnit().get();
  if (!unit)
    return;

  //  find which resource is available somewhere to collect, and we can also store it
  SMGameActorPtr actorToCollectFrom;
  uint resToCollect = 0;
  int numResChecked = 0;
  while (!actorToCollectFrom && numResChecked < resCollectCycle.size())
    {
    resToCollect = resCollectCycle[resCollectIdx];
    resCollectIdx++;
    if (resCollectIdx >= resCollectCycle.size())
      resCollectIdx = 0;
    numResChecked++;
    const int amountStoring = building->resourceCount(resToCollect, false, true) + building->reservedResourceSpaceCount(resToCollect);
    if (amountStoring < DEFAULT_RES_PER_STACK && building->canStoreResource(resToCollect, 1))
      {
      actorToCollectFrom = smGameContext->getGridMapHandler()->findClosestGriddedActor(gameContext, building->getEntryPosition(), 
        [&](SMGameActorPtr actor)
        {
        if (actor->getID() == building->getID())
          return false;
        const SMGameActorBlueprint* blueprint = smGameContext->getGameObjectFactory()->getGameActorBlueprint(actor->getBlueprintTypeID());
        if (!blueprint || blueprint->type != SMGameActorType::building)
          return false;
        //  todo: allow resource pickup from floor
        return actor->resourceCount(resToCollect) >= 1;
        });
      }
    }

  // then tell the idle unit to get this resource
  if (actorToCollectFrom)
    {
    UnitCommandPtr command(new CollectResourceUnitCommand(actorToCollectFrom->getID(), resToCollect));
    unit->performCommand(command);
    }
  }

void ComponentManufacturer::sendUnitToManufacture(GameContext* gameContext)
  {
  GameActorBuilding* building = getBuildingActor();
  GameActorUnit* unit = building->getIdleUnit().get();
  if (!unit)
    return;

  if (gotEnoughInputResources())
    {
    isUnitManufacturing = true;
    unit->performCommand(UnitCommandPtr(new ManufactureUnitCommand(blueprint)));
    }
  }