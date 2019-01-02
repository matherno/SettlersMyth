//SMInputHandler
// Created by matt on 3/12/17.
//

#include <RenderSystem/RenderableMesh.h>
#include <RenderSystem/PostProcessing.h>
#include <GameObjectDefs/StaticObjectDef.h>

#include "SMGameContext.h"
#include "Resources.h"
#include "SaveLoadFileHelper.h"
#include "Utils.h"
#include "GridMapAStar.h"

#define LAND_HEIGHT 0

bool SMGameContext::initialise()
  {
  bool success = GameContextImpl::initialise();
  getRenderContext()->registerDrawStage(DRAW_STAGE_POST_PROC_EDGE);
  getRenderContext()->registerDrawStage(DRAW_STAGE_OPAQUE_AFTER_EDGE);
  getRenderContext()->registerDrawStage(DRAW_STAGE_FOGOFWAR);

  selectionManager.reset(new WorldItemSelectionManager(getNextActorID()));
  addActor(selectionManager);

  gridMapHandler.reset(new GridMapAStar(GridXY(200, 200)));
  if (!gameObjectFactory.loadGameObjectDefs("gameobjdefs/"))
    return false;

//  PostProcStepHandlerPtr postProcSilhoutting(new PostProcSimpleBase(getRenderContext()->getNextPostProcessingStepID(), DRAW_STAGE_POST_PROC_EDGE, "shaders/SilhouttingFS.glsl"));
//  postProcSilhoutting->initialise(getRenderContext());
//  getRenderContext()->addPostProcessingStep(postProcSilhoutting);

  smInputHandler.reset(new SMInputHandler(getInputManager()->getNextHandlerID(), Vector3D(50, 0, -50), 50, 180, -45));
  addInputHandler(smInputHandler);


  initSurface();
  hudHandler.initialiseUI(this);
  pauseMenuHandler.reset(new PauseMenuHandler(getNextActorID()));
  addActor(pauseMenuHandler);


  return success;
  }

void SMGameContext::cleanUp()
  {
  dynamicActors.clear();
  staticActors.clear();
  if (surfaceMesh)
    {
    getRenderContext()->getRenderableSet()->removeRenderable(surfaceMesh->getID());
    surfaceMesh->cleanUp(getRenderContext());

    hudHandler.cleanUp(this);
    removeActor(pauseMenuHandler->getID());
    }
  GameContextImpl::cleanUp();
  }

void SMGameContext::processInputStage()
  {
  GameContextImpl::processInputStage();

  static const Vector3D lightDir = Vector3D(0.2, -0.5, -0.5).getUniform();
  double shadowMapOffset = smInputHandler->getZoomOffset();
  double shadowMapFOV = shadowMapOffset * 1.5;
  Vector3D shadowMapPos = smInputHandler->getFocalPosition() - lightDir * shadowMapOffset;
  static const uint shadowMapWidth = 1500;
  getRenderContext()->configureShadowMap(true, shadowMapPos, lightDir, shadowMapFOV, shadowMapOffset * 1.5, shadowMapWidth, shadowMapWidth);
  }

void SMGameContext::processUpdateStage()
  {
  hudHandler.updateUI(this);

  GameContextImpl::processUpdateStage();
  }

void SMGameContext::processDrawStage()
  {
  getRenderContext()->invalidateShadowMap();
  GameContextImpl::processDrawStage();
  }

void SMGameContext::initSurface()
  {
  RenderContext* renderContext = getRenderContext();

  const uint numCells = gridMapHandler->getMapSize().x;
  const float cellSize = 1;
  surfaceMesh.reset(new RenderableTerrain(renderContext->getNextRenderableID(), numCells, cellSize, DRAW_STAGE_OPAQUE_AFTER_EDGE));
  surfaceMesh->setMultiColour(colToVec3(50, 85, 25, true), colToVec3(60, 105, 30, true));
  surfaceMesh->initialise(renderContext);
  surfaceMesh->getTransform()->translate(0, 0, numCells * cellSize * -1);
  renderContext->getRenderableSet()->addRenderable(surfaceMesh);
  }

FontPtr SMGameContext::getDefaultFont()
  {
  return getRenderContext()->getSharedFont(FONT_DEFAULT_FNT, FONT_DEFAULT_GLYPHS, FONT_DEFAULT_SCALING);
  }

void SMGameContext::displayPauseMenu()
  {
  pauseMenuHandler->displayMenu(this);
  }

Vector3D SMGameContext::getCameraFocalPosition() const
  {
  return smInputHandler->getFocalPosition();
  }


SMGameActorPtr SMGameContext::createSMGameActor(uint gameObjDefID, const GridXY& position)
  {
  return createSMGameActor(gameObjDefID, position.centre());
  }

SMGameActorPtr SMGameContext::createSMGameActor(uint gameObjDefID, const Vector2D& position)
  {
  if (!gridMapHandler->isOnMap(position))
    return nullptr;

  auto gameActor = gameObjectFactory.createGameActor(this, gameObjDefID);
  if (gameActor)
    {
    auto staticActor = SMStaticActor::cast(gameActor.get());
    if (staticActor)
      staticActor->setGridPos(position);
    auto dynamicActor = SMDynamicActor::cast(gameActor.get());
    if (dynamicActor)
      dynamicActor->setPosition(position);
    addSMGameActor(gameActor);
    return gameActor;
    }
  return nullptr;
  }

void SMGameContext::destroySMActor(uint id)
  {
  SMGameActorPtr actor;

  SMStaticActorPtr staticActor = getStaticActor(id);
  if (staticActor)
    {
    staticActors.remove(id);
    const StaticObjectDef* staticObjDef = StaticObjectDef::cast(staticActor->getDef());
    gridMapHandler->setGridCells(0, staticActor->getGridPosition(), staticObjDef->getSize());
    actor = staticActor;
    }

  SMDynamicActorPtr dynamicActor = getDynamicActor(id);
  if (dynamicActor)
    {
    dynamicActors.remove(id);
    actor = dynamicActor;
    }

  if (actor)
    {
    gameObjectFactory.freeLinkID(actor->getLinkID());
    selectionManager->deselectTower(this, actor->getID());
    removeActor(actor->getID());
    }
  }

Vector2D SMGameContext::terrainHitTest(uint mouseX, uint mouseY)
  {
  Vector3D cursorWorldPos = getCursorWorldPos(mouseX, mouseY);
  Vector3D cursorViewDir = getViewDirectionAtCursor(mouseX, mouseY);
  ASSERT (cursorViewDir.y != 0, "View is parallel to terrain, can't hit!");
  double tValue = -1.0 * cursorWorldPos.y / cursorViewDir.y;
  Vector3D position = cursorWorldPos + (cursorViewDir * tValue);
  return Vector2D(position.x, -1 * position.z);
  }

bool SMGameContext::saveGame(string filePath)
  {
  mathernogl::logInfo("Saving game... " + filePath);
  XMLDocument doc;

  XMLElement* xmlSaveFile = xmlCreateElement(doc, nullptr, SL_SMSAVE);
  xmlSaveFile->SetAttribute(SL_VERSION, SAVE_CURRENT_VERSION);

  xmlCreateElement(doc, xmlSaveFile, SL_CAMERA_POS, smInputHandler->getFocalPosition());
  xmlCreateElement(doc, xmlSaveFile, SL_CAMERA_ZOOM, smInputHandler->getZoomOffset());
  xmlCreateElement(doc, xmlSaveFile, SL_CAMERA_ROT, smInputHandler->getRotation());
  xmlCreateElement(doc, xmlSaveFile, SL_CAMERA_PITCH, smInputHandler->getPitch());

  for (auto actor : *staticActors.getList())
    {
    if (actor->isSavingAllowed())
      {
      auto element = xmlCreateElement(doc, xmlSaveFile, SL_SMGAMEACTOR);
      actor->saveActor(element, this);
      }
    }

  for (auto actor : *dynamicActors.getList())
    {
    if (actor->isSavingAllowed())
      {
      auto element = xmlCreateElement(doc, xmlSaveFile, SL_SMGAMEACTOR);
      actor->saveActor(element, this);
      }
    }

  XMLError xmlError = doc.SaveFile(filePath.c_str());
  if (xmlError > 0)
    {
    mathernogl::logError("Failed to save to file '" + filePath + "' : " + XMLDocument::ErrorIDToName(xmlError));
    return false;
    }
  return true;
  }


bool SMGameContext::loadGame(string filePath)
  {
  mathernogl::logInfo("Loading game... " + filePath);
  XMLDocument doc;
  XMLError xmlError = doc.LoadFile(filePath.c_str());
  if (xmlError > 0)
    {
    mathernogl::logError("Failed to load file: " + std::string(XMLDocument::ErrorIDToName(xmlError)));
    return false;
    }

  XMLElement* xmlSaveFile = doc.FirstChildElement(SL_SMSAVE);
  if (!xmlSaveFile)
    {
    mathernogl::logError("Failed to load game: invalid save file format");
    return false;
    }

  Vector3D camPos = xmlGetVec3Value(xmlSaveFile, SL_CAMERA_POS);
  double camZoom = xmlGetDblValue(xmlSaveFile, SL_CAMERA_ZOOM);
  double camRot = xmlGetDblValue(xmlSaveFile, SL_CAMERA_ROT);
  double camPitch = xmlGetDblValue(xmlSaveFile, SL_CAMERA_PITCH);
  smInputHandler->setFocalPosition(camPos);
  smInputHandler->setZoomOffset(camZoom);
  smInputHandler->setRotation(camRot);
  smInputHandler->setPitch(camPitch);
  smInputHandler->setCameraNeedsRefresh();

  std::vector<SMGameActorPtr> loadedActors;
  XMLElement* xmlGameActor = xmlSaveFile->FirstChildElement(SL_SMGAMEACTOR);
  while (xmlGameActor)
    {
    SMGameActorPtr smGameActor = gameObjectFactory.createGameActor(this, xmlGameActor);
    addSMGameActor(smGameActor);
    loadedActors.push_back(smGameActor);
    xmlGameActor = xmlGameActor->NextSiblingElement(SL_SMGAMEACTOR);
    }
  for (SMGameActorPtr actor : loadedActors)
    actor->finaliseLoading(this);

  return true;
  }

void SMGameContext::addSMGameActor(SMGameActorPtr gameActor)
  {
  SMStaticActorPtr staticActor = std::dynamic_pointer_cast<SMStaticActor>(gameActor);
  if (staticActor)
    {
    staticActors.add(staticActor, staticActor->getID());
    auto staticObjectDef = dynamic_cast<const StaticObjectDef*>(staticActor->getDef());
    gridMapHandler->setGridCells(staticActor->getID(), staticActor->getGridPosition(), staticObjectDef->getSize());
    gridMapHandler->incrementMapVersion();
    }

  SMDynamicActorPtr dynamicActor = std::dynamic_pointer_cast<SMDynamicActor>(gameActor);
  if (dynamicActor)
    {
    dynamicActors.add(dynamicActor, dynamicActor->getID());
    }
  }

SMStaticActorPtr SMGameContext::getStaticActor(uint id)
  {
  if (staticActors.contains(id))
    return staticActors.get(id);
  return nullptr;
  }

SMDynamicActorPtr SMGameContext::getDynamicActor(uint id)
  {
  if (dynamicActors.contains(id))
    return dynamicActors.get(id);
  return nullptr;
  }

SMGameActorPtr SMGameContext::getSMGameActor(uint id)
  {
  if (auto actor = getStaticActor(id))
    return actor;
  if (auto actor = getDynamicActor(id))
    return actor;
  return nullptr;
  }

SMGameActorPtr SMGameContext::getSMGameActorByLinkID(uint linkID)
  {
  for (SMGameActorPtr actor : *staticActors.getList())
    {
    if (actor->getLinkID() == linkID)
      return actor;
    }
  for (SMGameActorPtr actor : *dynamicActors.getList())
    {
    if (actor->getLinkID() == linkID)
      return actor;
    }
  return nullptr;
  }

void SMGameContext::forEachSMActor(GameObjectType type, std::function<void(SMGameActorPtr)> func)
  {
  for (SMStaticActorPtr staticActor : *staticActors.getList())
    {
    if (gameObjectFactory.isTypeOrSubType(type, staticActor->getDef()->getType()))
      func(staticActor);
    }

  for (SMDynamicActorPtr dynamicActor: *dynamicActors.getList())
    {
    if (gameObjectFactory.isTypeOrSubType(type, dynamicActor->getDef()->getType()))
      func(dynamicActor);
    }
  }

void SMGameContext::dropResource(uint id, int amount, Vector2D position)
  {
  for (int i = 0; i < amount; ++i)
    createSMGameActor(id, position);
  }






