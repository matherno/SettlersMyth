//
// Created by matt on 3/12/17.
//

#include <RenderSystem/RenderableMesh.h>
#include <RenderSystem/PostProcessing.h>

#include "SMGameContext.h"
#include "Resources.h"

#define LAND_HEIGHT 0

bool TOGameContext::initialise()
  {
  bool success = GameContextImpl::initialise();
  getRenderContext()->registerDrawStage(DRAW_STAGE_POST_PROC_EDGE);
  getRenderContext()->registerDrawStage(DRAW_STAGE_OPAQUE_AFTER_EDGE);
  getRenderContext()->registerDrawStage(DRAW_STAGE_FOGOFWAR);

//  PostProcStepHandlerPtr postProcSilhoutting(new PostProcSimpleBase(getRenderContext()->getNextPostProcessingStepID(), DRAW_STAGE_POST_PROC_EDGE, "shaders/SilhouttingFS.glsl"));
//  postProcSilhoutting->initialise(getRenderContext());
//  getRenderContext()->addPostProcessingStep(postProcSilhoutting);

  if (loadedGameState)
    toInputHandler.reset(new TOInputHandler(getInputManager()->getNextHandlerID(), loadedGameState->cameraFocalPos, loadedGameState->cameraZoomFactor, loadedGameState->cameraRotation, -45));
  else
    toInputHandler.reset(new TOInputHandler(getInputManager()->getNextHandlerID(), Vector3D(10, 0, 10), 30, 0, -45));
  addInputHandler(toInputHandler);

  initSurface();
  hudHandler.initialiseUI(this);
  pauseMenuHandler.reset(new PauseMenuHandler(getNextActorID()));
  addActor(pauseMenuHandler);

  RenderableMesh* lumberer = new RenderableMesh(getRenderContext()->getNextRenderableID());
  lumberer->setMeshStorage(getRenderContext()->getSharedMeshStorage("resources/buildings/Woodcutter.obj"));
  lumberer->setDrawStyleVertColours();
  lumberer->getTransform()->translate(Vector3D(10, 0, 10));
  lumberer->initialise(getRenderContext());
  getRenderContext()->getRenderableSet()->addRenderable(RenderablePtr(lumberer));

  return success;
  }

void TOGameContext::cleanUp()
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

void TOGameContext::processInputStage()
  {
  GameContextImpl::processInputStage();

  static const Vector3D lightDir = Vector3D(0.2, -0.5, -0.5).getUniform();
  double shadowMapOffset = toInputHandler->getZoomOffset();
  double shadowMapFOV = shadowMapOffset * 1.5;
  Vector3D shadowMapPos = toInputHandler->getFocalPosition() - lightDir * shadowMapOffset;
  static const uint shadowMapWidth = 1500;
  getRenderContext()->configureShadowMap(false, shadowMapPos, lightDir, shadowMapFOV, shadowMapOffset * 1.5, shadowMapWidth, shadowMapWidth);
  }

void TOGameContext::processUpdateStage()
  {
  hudHandler.updateUI(this);

  GameContextImpl::processUpdateStage();
  }

void TOGameContext::processDrawStage()
  {
  getRenderContext()->invalidateShadowMap();
  GameContextImpl::processDrawStage();
  }

void TOGameContext::initSurface()
  {
  RenderContext* renderContext = getRenderContext();

  const float cellSize = 1;
  const uint numCells = 200;
  surfaceMesh.reset(new RenderableTerrain(renderContext->getNextRenderableID(), numCells, cellSize, DRAW_STAGE_OPAQUE_AFTER_EDGE));
  surfaceMesh->setMultiColour(Vector3D(pow(0.2, 2.2), pow(0.4, 2.2), pow(0.2, 2.2)), Vector3D(pow(0.15, 2.2), pow(0.3, 2.2), pow(0.15, 2.2)));
  surfaceMesh->initialise(renderContext);
  renderContext->getRenderableSet()->addRenderable(surfaceMesh);
  }

FontPtr TOGameContext::getDefaultFont()
  {
  return getRenderContext()->getSharedFont(FONT_DEFAULT_FNT, FONT_DEFAULT_GLYPHS, FONT_DEFAULT_SCALING);
  }

void TOGameContext::getGameState(TOGameState* state)
  {
  state->cameraFocalPos = toInputHandler->getFocalPosition();
  state->cameraRotation = toInputHandler->getRotation();
  state->cameraZoomFactor = toInputHandler->getZoomOffset();
  }

void TOGameContext::displayPauseMenu()
  {
  pauseMenuHandler->displayMenu(this);
  }

Vector3D TOGameContext::getCameraFocalPosition() const
  {
  return toInputHandler->getFocalPosition();
  }


