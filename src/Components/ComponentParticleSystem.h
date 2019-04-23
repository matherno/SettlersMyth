#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>

/*
*   
*/


class ComponentParticleSystemBlueprint : public SMComponentBlueprint
  {
public:
  Vector3D particleColour;
  double particleSize = 3;
  double gravityEffect = 0.001;
  double initialVelocity = 0.8;
  int timeAlive = 1000;
  int timeBetweenSpawns = 50;
  bool isDirectionRandom = false;
  bool isDirectionHemisphere = false;
  bool isDirectionSingle = false;
  double hemisphereExp = 20;
  Vector3D spawnDirection = Vector3D(0, 1, 0);
  bool isSpawnPoint = false;
  bool isSpawnSphere = false;
  bool isSpawnCircle = false;
  bool isSpawnLine = false;
  double spawnRadius = 1;
  Vector3D spawnCircleNormal = Vector3D(0, 1, 0);
  Vector3D spawnLinePt1 = Vector3D(0, 0, 0);
  Vector3D spawnLinePt2 = Vector3D(1, 0, 0);
  
  std::vector<Vector3D> emitters;
  enum 
    {
    CONSTANT,
    CONSTRUCTED,
    } trigger;

public:
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };


class ParticleSystem;

class ComponentParticleSystem : public SMComponent
  {
private:
  const ComponentParticleSystemBlueprint* blueprint;

protected:
  std::shared_ptr<ParticleSystem> particleSystem;

public:
  ComponentParticleSystem(const SMGameActorPtr& actor, SMComponentType type, const ComponentParticleSystemBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;
  virtual void onMessage(GameContext* gameContext, SMMessage message, void* extra) override;
  };
