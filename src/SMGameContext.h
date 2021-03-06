#pragma once

class SMGameContext;

#include <GameSystem/GameSystem.h>
#include <GameSystem/GameContextImpl.h>
#include <ParticleSystem/ParticleSystem.h>
#include <RenderSystem/RenderableTerrain.h>
#include <GameSystem/Timer.h>
#include "SMInputHandler.h"
#include "HUDHandler.h"
#include "PauseMenuHandler.h"
#include "WorldItemSelectionManager.h"
#include "SMSettings.h"
#include "SMGameActor.h"
#include "SMGameObjectFactory.h"
#include "Grid.h"
#include "GridMapHandler.h"

#define DRAW_STAGE_SHADOW                (DRAW_STAGE_OPAQUE + 1)
#define DRAW_STAGE_NO_SHADOW_CASTING     (DRAW_STAGE_SHADOW + 1)

/*
*   Sub-class of Game Context to capture the central state of the TowOff game
*/

typedef mathernogl::MappedList<std::pair<Vector3D, float>> VisibilityMarkersList;

class SMGameContext : public GameContextImpl
  {
private:
  std::shared_ptr<RenderableTerrain> surfaceMesh;
  HUDHandler hudHandler;
  std::shared_ptr<SMSettings> settingsHandler;
  std::shared_ptr<SMInputHandler> smInputHandler;
  std::shared_ptr<PauseMenuHandler> pauseMenuHandler;
  std::shared_ptr<WorldItemSelectionManager> selectionManager;
  GameObjectFactory gameObjectFactory;
  std::unique_ptr<GridMapHandler> gridMapHandler;

  mathernogl::MappedList<SMGameActorPtr> smActorsList;
  Timer refreshShadowMapTimer;
  bool shadowMapValid = false;

public:
  SMGameContext(const RenderContextPtr& renderContext, std::shared_ptr<SMSettings> settings)
    : GameContextImpl(renderContext), settingsHandler(settings) {}
  virtual bool initialise() override;
  virtual void cleanUp() override;
  virtual void processInputStage() override;
  virtual void processUpdateStage() override;
  virtual void processDrawStage() override;
  virtual void doIMGui() override;

  virtual FontPtr getDefaultFont() override;
  HUDHandler* getHUDHandler(){ return &hudHandler; }
  SMSettings* getSettings(){ return settingsHandler.get(); }
  const GameObjectFactory* getGameObjectFactory() const { return &gameObjectFactory; }
  const GridMapHandler* getGridMapHandler() const { return gridMapHandler.get(); }
  WorldItemSelectionManager* getSelectionManager() { return selectionManager.get(); }
  Vector3D getCameraFocalPosition() const;
  Vector2D terrainHitTest(uint mouseX, uint mouseY);
  void invalidateShadowMap();
  void recalculateShadowMap();

  void displayPauseMenu();
  void dropResource(uint id, int amount, Vector2D position);
  bool saveGame(string filePath);
  bool loadGame(string filePath);

  SMGameActorPtr createSMGameActor(uint blueprintID, const GridXY& position);
  SMGameActorPtr createSMGameActor(uint blueprintID, const Vector2D& position);
  SMGameActorPtr getSMGameActor(uint id);
  SMGameActorPtr getSMGameActorByLinkID(uint linkID);
  uint getSMActorCount() const;
  void destroySMActor(uint id);
  void forEachSMActor(std::function<void(SMGameActorPtr actor)> func);

  inline static SMGameContext* cast(GameContext* context)
    {
    SMGameContext* toContext = dynamic_cast<SMGameContext*>(context);
    ASSERT(toContext, "Given game context wasn't a SMGameContext!");
    return toContext;
    }

protected:
  void initSurface();
  void addSMGameActor(SMGameActorPtr gameActor);
  };
