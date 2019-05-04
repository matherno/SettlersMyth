//SMInputHandler
// Created by matt on 3/12/17.
//

#include <RenderSystem/RenderableMesh.h>
#include <RenderSystem/PostProcessing.h>

#include "SMGameContext.h"
#include "Resources.h"
#include "SaveLoadFileHelper.h"
#include "Utils.h"
#include "imgui/imgui.h"

#define LAND_HEIGHT 0

#define DEFAULT_MAP_SIZE 200

bool SMGameContext::initialise()
  {
  bool success = GameContextImpl::initialise();
  getRenderContext()->setShadowMapDrawStage(DRAW_STAGE_SHADOW);
  getRenderContext()->registerDrawStage(DRAW_STAGE_NO_SHADOW_CASTING);
  getRenderContext()->getVoxelBatchManager()->setVoxelSize(VOXEL_SIZE);
  getRenderContext()->getVoxelBatchManager()->setBatchSize(VOXEL_SIZE * 200);

  selectionManager.reset(new WorldItemSelectionManager(getNextActorID()));
  addActor(selectionManager);

  gridMapHandler.reset(new GridMapHandler(GridXY(DEFAULT_MAP_SIZE, DEFAULT_MAP_SIZE)));
  if (!gameObjectFactory.loadBlueprints(this, "blueprints/"))
    return false;


  smInputHandler.reset(new SMInputHandler(getInputManager()->getNextHandlerID(), Vector3D(DEFAULT_MAP_SIZE / 2.0, 0, -DEFAULT_MAP_SIZE / 2.0), 50, 135, -45));
  addInputHandler(smInputHandler);


  initSurface();
  hudHandler.initialiseUI(this);
  pauseMenuHandler.reset(new PauseMenuHandler(getNextActorID()));
  addActor(pauseMenuHandler);

  invalidateShadowMap();

  return success;
  }

void SMGameContext::cleanUp()
  {
  smActorsList.clear();
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
  }

void SMGameContext::processUpdateStage()
  {
  hudHandler.updateUI(this);

  GameContextImpl::processUpdateStage();
  }

void SMGameContext::processDrawStage()
  {
  if (refreshShadowMapTimer.incrementTimer(getDeltaTime()) && !shadowMapValid)
    recalculateShadowMap();
  GameContextImpl::processDrawStage();
  }

void SMGameContext::doIMGui()
  {
  GameContextImpl::doIMGui();

#ifndef NDEBUG
  const double msPerFrame = 1000.0f / ImGui::GetIO().Framerate;

  static std::list<float> runningFrameTimeList;
  runningFrameTimeList.push_back(msPerFrame);
  if (runningFrameTimeList.size() > 1000)
    runningFrameTimeList.pop_front();

  std::vector<float> graphPoints(runningFrameTimeList.begin(), runningFrameTimeList.end());

  ImGui::Begin("Debug Panel");
  ImGui::Text("FPS %.1f, ms per frame %.3f", ImGui::GetIO().Framerate, msPerFrame);
  ImGui::PlotLines("", graphPoints.data(), graphPoints.size(), 0, nullptr, 15, 30, ImVec2(0, 100));
  ImGui::End();
#endif
  }

void SMGameContext::initSurface()
  {
  RenderContext* renderContext = getRenderContext();

  const uint numCells = gridMapHandler->getMapSize().x;
  const float cellSize = 1;
  surfaceMesh.reset(new RenderableTerrain(renderContext->getNextRenderableID(), numCells, cellSize));
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

SMGameActorPtr SMGameContext::createSMGameActor(uint blueprintID, const GridXY& position)
  {
  const bool isPosInCentre = gameObjectFactory.getGameActorBlueprint(blueprintID)->isPosInCentre;
  const Vector2D position2D = isPosInCentre ? position.centre() : (Vector2D) position;
  return createSMGameActor(blueprintID, position2D);
  }

SMGameActorPtr SMGameContext::createSMGameActor(uint blueprintID, const Vector2D& position)
  {
  if (!gridMapHandler->isOnMap(position))
    return nullptr;

  SMGameActorPtr gameActor = gameObjectFactory.createGameActor(getNextActorID(), blueprintID);
  if (gameActor)
    {
    gameActor->setPosition(position);
    addSMGameActor(gameActor);
    return gameActor;
    }
  return nullptr;
  }

void SMGameContext::destroySMActor(uint id)
  {
  if(SMGameActorPtr actor = getSMGameActor(id))
    {
    smActorsList.remove(id);
    gridMapHandler->setGridCells(actor->getGridPosition(), actor->getSize(), 0, false);
    gameObjectFactory.freeLinkID(actor->getLinkID());
    selectionManager->deselectActor(this, actor->getID());
    removeActor(actor->getID());
    getRenderContext()->invalidateShadowMap();
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

void SMGameContext::recalculateShadowMap()
  {
  if (settingsHandler->enableShadows())
    {
    static const Vector3D lightDir = Vector3D(-1.5, -3, -1).getUniform();
    double shadowMapOffset = smInputHandler->getZoomOffset();
    double shadowMapFOV = shadowMapOffset * 1.5;
    Vector3D shadowMapPos = smInputHandler->getFocalPosition() - lightDir * shadowMapOffset;
    static const uint shadowMapWidth = 1500;
    getRenderContext()->configureShadowMap(true, shadowMapPos, lightDir, shadowMapFOV, shadowMapOffset * 1.5, shadowMapWidth, shadowMapWidth);
    }
  else
    {
    getRenderContext()->configureShadowMap(false);
    }
  shadowMapValid = true;
  refreshShadowMapTimer.setTimeOut(100);
  refreshShadowMapTimer.reset();
  }

void SMGameContext::invalidateShadowMap()
  {
  shadowMapValid = false;
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

  for (auto actor : *smActorsList.getList())
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
    SMGameActorPtr smGameActor = gameObjectFactory.createGameActor(getNextActorID(), xmlGameActor);
    addSMGameActor(smGameActor);
    gameObjectFactory.registerLinkID(smGameActor->getLinkID());
    loadedActors.push_back(smGameActor);
    xmlGameActor = xmlGameActor->NextSiblingElement(SL_SMGAMEACTOR);
    }
  for (SMGameActorPtr actor : loadedActors)
    actor->finaliseLoading(this);

  return true;
  }

void SMGameContext::addSMGameActor(SMGameActorPtr gameActor)
  {
  addActor(gameActor);
  smActorsList.add(gameActor, gameActor->getID());
   
  gridMapHandler->startGridTransaction();
  gridMapHandler->setGridCells(gameActor->getGridPosition(), gameActor->getSize(), gameActor->getID(), true);
  const SMGameActorBlueprint* blueprint = gameObjectFactory.getGameActorBlueprint(gameActor->getBlueprintTypeID());
  for (GridXY pos : blueprint->clearGridCells)
    gridMapHandler->setGridCellIsObstacle(gameActor->getGridPosition() + pos, false);
  gridMapHandler->endGridTransaction();

  getRenderContext()->invalidateShadowMap();
  }

SMGameActorPtr SMGameContext::getSMGameActor(uint id)
  {
  if (smActorsList.contains(id))
    return smActorsList.get(id);
  return nullptr;
  }

SMGameActorPtr SMGameContext::getSMGameActorByLinkID(uint linkID)
  {
  for (SMGameActorPtr actor : *smActorsList.getList())
    {
    if (actor->getLinkID() == linkID)
      return actor;
    }
  return nullptr;
  }

void SMGameContext::forEachSMActor(std::function<void(SMGameActorPtr)> func)
  {
  for (SMGameActorPtr& actor : *smActorsList.getList())
    func(actor);
  }

void SMGameContext::dropResource(uint id, int amount, Vector2D position)
  {
  for (int i = 0; i < amount; ++i)
    createSMGameActor(id, position);
  }

uint SMGameContext::getSMActorCount() const
  {
  return (uint)smActorsList.count();
  }







