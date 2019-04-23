//
// Created by matt on 25/02/19.
//

#include <SMGameContext.h>
#include <BlueprintFileHelper.h>
#include "ComponentParticleSystem.h"
#include "ParticleSystem/ParticleSystem.h"
#include "GameActorTypes/GameActorBuilding.h"

/*
*   ComponentParticleSystemBlueprint
*/

bool ComponentParticleSystemBlueprint::loadFromXML(XMLElement* xmlComponent, string* errorMsg)
  {
  XMLElement* xmlModel = xmlComponent->FirstChildElement(OD_MODEL);

  particleColour = xmlGetRGBValue(xmlComponent, OD_COLOUR);
  particleSize = xmlComponent->DoubleAttribute(OD_SIZE, 3);
  gravityEffect = xmlComponent->DoubleAttribute(OD_GRAVITY, 0.001) / 1000.0;
  initialVelocity = xmlComponent->DoubleAttribute(OD_VELOCITY, 0.8) / 1000.0;
  timeAlive = xmlComponent->IntAttribute(OD_TIMEALIVE, 2000);
  timeBetweenSpawns = xmlComponent->IntAttribute(OD_TIMEBETWEENSPAWNS, 50);

  XMLElement* xmlEmitter = xmlComponent->FirstChildElement(OD_EMITTER);
  while (xmlEmitter)
    {
    emitters.push_back(xmlGetVec3Value(xmlEmitter));
    xmlEmitter = xmlEmitter->NextSiblingElement(OD_EMITTER);  
    }

  XMLElement* xmlSpawnPos = xmlComponent->FirstChildElement(OD_SPAWNPOS);
  if (xmlSpawnPos)
    {
    const string spawnMode = xmlGetStringAttribute(xmlSpawnPos, OD_MODE);
    if (spawnMode == OD_POINT)
      isSpawnPoint = true;
    else if (spawnMode == OD_LINE)
      isSpawnLine = true;
    else if (spawnMode == OD_SPHERE)
      isSpawnSphere = true;
    else if (spawnMode == OD_CIRCLE)
      isSpawnCircle = true;

    spawnRadius = xmlSpawnPos->DoubleAttribute(OD_RADIUS, 1);
    spawnLinePt1 = xmlGetVec3Value(xmlSpawnPos, OD_POINT1);
    spawnLinePt2 = xmlGetVec3Value(xmlSpawnPos, OD_POINT2);
    spawnCircleNormal = xmlGetVec3Value(xmlSpawnPos, OD_NORMAL);
    }
  
  XMLElement* xmlSpawnDir = xmlComponent->FirstChildElement(OD_SPAWNDIR);
  if (xmlSpawnDir)
    {
    const string spawnMode = xmlGetStringAttribute(xmlSpawnDir, OD_MODE);
    if (spawnMode == OD_RANDOM)
      isDirectionRandom = true;
    else if (spawnMode == OD_DIRECTION)
      isDirectionSingle = true;
    else if (spawnMode == OD_HEMISPHERE)
      isDirectionHemisphere = true;

    hemisphereExp = xmlSpawnDir->DoubleAttribute(OD_EXP, 20);
    spawnDirection = xmlGetVec3Value(xmlSpawnDir, OD_DIRECTION);
    }

  XMLElement* xmlTrigger = xmlComponent->FirstChildElement(OD_TRIGGER);
  if (xmlTrigger)
    {
    const string triggerMode = xmlGetStringAttribute(xmlTrigger, OD_MODE);
    if (triggerMode == OD_CONSTANT)
      trigger = CONSTANT;
    else if (triggerMode == OD_CONSTRUCTED)
      trigger = CONSTRUCTED;
    }
  

  return true;
  }

SMComponentPtr ComponentParticleSystemBlueprint::constructComponent(SMGameActorPtr actor) const
  {
  return SMComponentPtr(new ComponentParticleSystem(actor, type, this));
  }





/*
*   ComponentParticleSystem
*/

ComponentParticleSystem::ComponentParticleSystem(const SMGameActorPtr& actor, SMComponentType type, const ComponentParticleSystemBlueprint* blueprint)
    : SMComponent(actor, type), blueprint(blueprint)
  {
  }

void ComponentParticleSystem::initialise(GameContext* gameContext)
  {
  /*
  *   Setup the particle system
  */
  particleSystem.reset(new ParticleSystem(gameContext->getNextActorID(), false));
  particleSystem->setGravityAccel(blueprint->gravityEffect);
  particleSystem->setInitVelocity(blueprint->initialVelocity);
  particleSystem->setParticleSize(blueprint->particleSize);
  particleSystem->setTimeAlive(blueprint->timeAlive);
  particleSystem->setTimeBetweenSpawns(blueprint->timeBetweenSpawns);
  particleSystem->setTranslation(getActor()->getPosition3D());

  if (blueprint->isSpawnCircle)
    particleSystem->setParticleSpawnCircle(blueprint->spawnRadius, blueprint->spawnCircleNormal);
  else if (blueprint->isSpawnLine)
    particleSystem->setParticleSpawnLine(blueprint->spawnLinePt1, blueprint->spawnLinePt1);
  else if (blueprint->isSpawnSphere)
    particleSystem->setParticleSpawnSphere(blueprint->spawnRadius);
  else
    particleSystem->setParticleSpawnPoint();

  if (blueprint->isDirectionSingle)
    particleSystem->setParticleDirection(blueprint->spawnDirection);
  else if (blueprint->isDirectionHemisphere)
    particleSystem->setParticleDirectionHemisphere(blueprint->spawnDirection, blueprint->hemisphereExp);
  else
    particleSystem->setParticleDirectionRandom();

  gameContext->addActor(particleSystem);


  /*
  *   Create the listed emitters
  */
  for (Vector3D emitter : blueprint->emitters)
    particleSystem->addEmitter(emitter, 99999999999, blueprint->particleColour);


  /*
  *   Only start the particle emitters if trigger allows it
  */
  SMGameActorPtr actor = getActor();
  GameActorBuilding* actorBuilding = GameActorBuilding::cast(actor.get());
  bool startEmitters = false;
  if(blueprint->trigger == ComponentParticleSystemBlueprint::CONSTRUCTED)
    {
    if (actorBuilding && !actorBuilding->getIsUnderConstruction())
      startEmitters = true;
    }
  else if (blueprint->trigger == ComponentParticleSystemBlueprint::CONSTANT)
    {
    startEmitters = true;
    }

  if (!startEmitters)
    particleSystem->stopEmitters();
  }

void ComponentParticleSystem::update(GameContext* gameContext)
  {
  }

void ComponentParticleSystem::cleanUp(GameContext* gameContext)
  {
  if (particleSystem)
    {
    gameContext->removeActor(particleSystem->getID());
    particleSystem.reset();
    }
  }

void ComponentParticleSystem::onMessage(GameContext* gameContext, SMMessage message, void* extra)
  {
  switch (message)
    {
    case SMMessage::actorRotationChanged:
    case SMMessage::actorPositionChanged:
      particleSystem->setTranslation(getActor()->getPosition3D());
      break;
    case SMMessage::constructionFinished:
      if (blueprint->trigger == ComponentParticleSystemBlueprint::CONSTRUCTED)
        particleSystem->startEmitters();
      break;
    }
  }
