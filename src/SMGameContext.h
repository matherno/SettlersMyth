#pragma once

class SMGameContext;

#include <GameSystem/GameSystem.h>
#include <GameSystem/GameContextImpl.h>
#include <ParticleSystem/ParticleSystem.h>
#include <RenderSystem/RenderableTerrain.h>
#include <GameSystem/Timer.h>
#include "SMInputHandler.h"
#include "HUDHandler.h"
#include "SMGameSaveLoad.h"
#include "PauseMenuHandler.h"
#include "WorldItemSelectionManager.h"
#include "SMSettings.h"
#include "SMGameActor.h"
#include "SMGameObjectFactory.h"
#include "Grid.h"

#define DRAW_STAGE_POST_PROC_EDGE     (DRAW_STAGE_OPAQUE + 1)
#define DRAW_STAGE_OPAQUE_AFTER_EDGE  (DRAW_STAGE_POST_PROC_EDGE + 1)
#define DRAW_STAGE_FOGOFWAR           (DRAW_STAGE_TRANSPARENT + 1)

/*
*   Sub-class of Game Context to capture the central state of the TowOff game
*/

typedef mathernogl::MappedList<std::pair<Vector3D, float>> VisibilityMarkersList;

class SMGameContext : public GameContextImpl
  {
private:
  std::shared_ptr<SMGameState> loadedGameState;
  std::shared_ptr<RenderableTerrain> surfaceMesh;
  HUDHandler hudHandler;
  std::shared_ptr<SMSettings> settingsHandler;
  std::shared_ptr<SMInputHandler> toInputHandler;
  std::shared_ptr<PauseMenuHandler> pauseMenuHandler;
  GameObjectFactory gameObjectFactory;

  mathernogl::MappedList<SMStaticActorPtr> staticActors;
  mathernogl::MappedList<SMResourceActorPtr> resourceActors;
  std::vector<uint> griddedActorIDs;
  GridXY mapSize;

public:
  SMGameContext(const RenderContextPtr& renderContext, std::shared_ptr<SMSettings> settings, std::shared_ptr<SMGameState> loadedGameState = nullptr)
    : GameContextImpl(renderContext), settingsHandler(settings), loadedGameState(loadedGameState) {}
  virtual bool initialise() override;
  virtual void cleanUp() override;
  virtual void processInputStage() override;
  virtual void processUpdateStage() override;
  virtual void processDrawStage() override;

  virtual FontPtr getDefaultFont() override;
  HUDHandler* getHUDHandler(){ return &hudHandler; }
  SMSettings* getSettings(){ return settingsHandler.get(); }
  void displayPauseMenu();
  void getGameState(SMGameState* state);
  const GameObjectFactory* getGameObjectFactory() const { return &gameObjectFactory; }

  Vector3D getCameraFocalPosition() const;
  Vector2D terrainHitTest(uint mouseX, uint mouseY);
  SMStaticActorPtr getObjectAtGridPos(const GridXY& gridPos);
  bool isOnMap(const GridXY& gridPos);
  bool isCellClear(const GridXY& gridPos);
  bool isRegionClear(const GridXY& gridPos, const GridXY& regionSize);

  SMGameActorPtr createSMGameActor(uint gameObjDefID, const GridXY& position);

  inline static SMGameContext* cast(GameContext* context)
    {
    SMGameContext* toContext = dynamic_cast<SMGameContext*>(context);
    ASSERT(toContext, "Given game context wasn't a SMGameContext!");
    return toContext;
    }

protected:
  void initSurface();
  void setGridCells(uint id, const GridXY& gridPos, const GridXY& regionSize);
  };
