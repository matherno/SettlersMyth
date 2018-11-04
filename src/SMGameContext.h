#pragma once

class TOGameContext;

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

#define DRAW_STAGE_POST_PROC_EDGE     (DRAW_STAGE_OPAQUE + 1)
#define DRAW_STAGE_OPAQUE_AFTER_EDGE  (DRAW_STAGE_POST_PROC_EDGE + 1)
#define DRAW_STAGE_FOGOFWAR           (DRAW_STAGE_TRANSPARENT + 1)

/*
*   Sub-class of Game Context to capture the central state of the TowOff game
*/

typedef mathernogl::MappedList<std::pair<Vector3D, float>> VisibilityMarkersList;

class TOGameContext : public GameContextImpl
  {
private:
  std::shared_ptr<TOGameState> loadedGameState;
  std::shared_ptr<RenderableTerrain> surfaceMesh;
  HUDHandler hudHandler;
  std::shared_ptr<SMSettings> settingsHandler;
  std::shared_ptr<TOInputHandler> toInputHandler;
  std::shared_ptr<PauseMenuHandler> pauseMenuHandler;

public:
  TOGameContext(const RenderContextPtr& renderContext, std::shared_ptr<SMSettings> settings, std::shared_ptr<TOGameState> loadedGameState = nullptr)
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
  void getGameState(TOGameState* state);

  Vector3D getCameraFocalPosition() const;

  inline static TOGameContext* cast(GameContext* context)
    {
    TOGameContext* toContext = dynamic_cast<TOGameContext*>(context);
    ASSERT(toContext, "Given game context wasn't a TOGameContext!");
    return toContext;
    }

protected:
  void initSurface();
  };
