//
// Created by matt on 3/12/17.
//

#include <RenderSystem/RenderableMesh.h>
#include <RenderSystem/PostProcessing.h>
#include <GameObjectDefs/StaticObjectDef.h>

#include "SMGameContext.h"
#include "Resources.h"
#include "SaveLoadFileHelper.h"

#define LAND_HEIGHT 0

bool SMGameContext::initialise()
  {
  bool success = GameContextImpl::initialise();
  getRenderContext()->registerDrawStage(DRAW_STAGE_POST_PROC_EDGE);
  getRenderContext()->registerDrawStage(DRAW_STAGE_OPAQUE_AFTER_EDGE);
  getRenderContext()->registerDrawStage(DRAW_STAGE_FOGOFWAR);

  if (!gameObjectFactory.loadGameObjectDefs("gameobjdefs/"))
    return false;

//  PostProcStepHandlerPtr postProcSilhoutting(new PostProcSimpleBase(getRenderContext()->getNextPostProcessingStepID(), DRAW_STAGE_POST_PROC_EDGE, "shaders/SilhouttingFS.glsl"));
//  postProcSilhoutting->initialise(getRenderContext());
//  getRenderContext()->addPostProcessingStep(postProcSilhoutting);

  smInputHandler.reset(new SMInputHandler(getInputManager()->getNextHandlerID(), Vector3D(50, 0, -50), 40, 0, -45));
  addInputHandler(smInputHandler);

  initSurface();
  hudHandler.initialiseUI(this);
  pauseMenuHandler.reset(new PauseMenuHandler(getNextActorID()));
  addActor(pauseMenuHandler);

  griddedActorIDs.clear();
  for (int x = 0; x < mapSize.x; ++x)
    {
    for (int y = 0; y < mapSize.y; ++y)
      griddedActorIDs.push_back(0);
    }

  return success;
  }

void SMGameContext::cleanUp()
  {
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
  getRenderContext()->configureShadowMap(false, shadowMapPos, lightDir, shadowMapFOV, shadowMapOffset * 1.5, shadowMapWidth, shadowMapWidth);
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

  const float cellSize = 1;
  const uint numCells = 200;
  mapSize.x = numCells;
  mapSize.y = numCells;
  surfaceMesh.reset(new RenderableTerrain(renderContext->getNextRenderableID(), numCells, cellSize, DRAW_STAGE_OPAQUE_AFTER_EDGE));
  surfaceMesh->setMultiColour(Vector3D(pow(0.2, 2.2), pow(0.4, 2.2), pow(0.2, 2.2)), Vector3D(pow(0.15, 2.2), pow(0.3, 2.2), pow(0.15, 2.2)));
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
  if (!isOnMap(position))
    return nullptr;

  auto gameActor = gameObjectFactory.createGameActor(this, gameObjDefID);
  if (gameActor)
    {
    addSMGameActor(gameActor);
    auto staticActor = SMStaticActor::cast(gameActor.get());
    if (staticActor)
      staticActor->setGridPos(position);
    return gameActor;
    }
  return nullptr;
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

SMStaticActorPtr SMGameContext::getObjectAtGridPos(const GridXY& gridPos)
  {
  if (!isOnMap(gridPos))
    return nullptr;

  auto actorID = griddedActorIDs[gridPos.x + gridPos.y * mapSize.x];
  if (actorID > 0 && staticActors.contains(actorID))
    return staticActors.get(actorID);
  return nullptr;
  }

bool SMGameContext::isOnMap(const GridXY& gridPos)
  {
  return gridPos.x >= 0 && gridPos.x < mapSize.x && gridPos.y >= 0 && gridPos.y < mapSize.y;
  }

bool SMGameContext::isCellClear(const GridXY& gridPos)
  {
  if (!isOnMap(gridPos))
    return false;
  return griddedActorIDs[gridPos.x + gridPos.y * mapSize.x] == 0;
  }

bool SMGameContext::isRegionClear(const GridXY& gridPos, const GridXY& regionSize)
  {
  for (int x = 0; x < regionSize.x; ++x)
    {
    for (int y = 0; y < regionSize.y; ++y)
      {
      if (!isCellClear(gridPos + GridXY(x, y)))
        return false;
      }
    }
  return true;
  }

void SMGameContext::setGridCells(uint id, const GridXY& gridPos, const GridXY& regionSize)
  {
  for (int x = 0; x < regionSize.x; ++x)
    {
    for (int y = 0; y < regionSize.y; ++y)
      {
      GridXY pos = gridPos + GridXY(x, y);
      if (isOnMap(pos))
        griddedActorIDs[pos.x + pos.y * mapSize.x] = id;
      }
    }
  }

bool SMGameContext::saveGame(string filePath) const
  {
  mathernogl::logInfo("Saving game... " + filePath);
  XMLDocument doc;

  XMLElement* xmlSaveFile = xmlCreateElement(doc, nullptr, SL_SMSAVE);
  xmlSaveFile->SetAttribute(SL_VERSION, SAVE_CURRENT_VERSION);

  xmlCreateElement(doc, xmlSaveFile, SL_CAMERA_POS, smInputHandler->getFocalPosition());
  xmlCreateElement(doc, xmlSaveFile, SL_CAMERA_ZOOM, smInputHandler->getZoomOffset());
  xmlCreateElement(doc, xmlSaveFile, SL_CAMERA_ROT, smInputHandler->getRotation());

  for (auto actor : *staticActors.getList())
    {
    auto element = xmlCreateElement(doc, xmlSaveFile, SL_SMGAMEACTOR);
    actor->saveActor(element);
    }

  for (auto actor : *resourceActors.getList())
    {
    auto element = xmlCreateElement(doc, xmlSaveFile, SL_SMGAMEACTOR);
    actor->saveActor(element);
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
  smInputHandler->setFocalPosition(camPos);
  smInputHandler->setZoomOffset(camZoom);
  smInputHandler->setRotation(camRot);
  smInputHandler->setCameraNeedsRefresh();

  XMLElement* xmlGameActor = xmlSaveFile->FirstChildElement(SL_SMGAMEACTOR);
  while (xmlGameActor)
    {
    auto smGameActor = gameObjectFactory.createGameActor(this, xmlGameActor);
    addSMGameActor(smGameActor);
    xmlGameActor = xmlGameActor->NextSiblingElement(SL_SMGAMEACTOR);
    }

  return true;
  }

void SMGameContext::addSMGameActor(SMGameActorPtr gameActor)
  {
  SMStaticActorPtr staticActor = std::dynamic_pointer_cast<SMStaticActor>(gameActor);
  if (staticActor)
    {
    staticActors.add(staticActor, staticActor->getID());
    auto staticObjectDef = dynamic_cast<const StaticObjectDef*>(staticActor->getDef());
    setGridCells(staticActor->getID(), staticActor->getGridPosition(), staticObjectDef->getSize());
    }

  SMResourceActorPtr resourceActor = std::dynamic_pointer_cast<SMResourceActor>(gameActor);
  if (resourceActor)
    {
    resourceActors.add(resourceActor, resourceActor->getID());
    }
  }






