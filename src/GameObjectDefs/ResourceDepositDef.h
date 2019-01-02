#pragma once

#include "StaticObjectDef.h"

/*
*   
*/

class ResourceDepositDef : public StaticObjectDef
  {
private:
  string resourceName;
  uint resourceAmount = 0;

public:
  virtual bool loadFromXML(tinyxml2::XMLElement* xmlGameObjectDef, string* errorMsg) override;
  virtual GameObjectType getType() const override { return GameObjectType::deposit; }

  virtual SMGameActorPtr createGameActor(GameContext* gameContext) const override;
protected:
  virtual void createActorBehaviours(std::vector<IGameObjectBehaviourPtr>* behaviourList) const override;
  };


class ResourceDepositeBehaviour : public IGameObjectBehaviour
  {
public:
  virtual void initialise(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void update(SMGameActor* gameActor, GameContext* gameContext) override;
  virtual void cleanUp(SMGameActor* gameActor, GameContext* gameContext) override;
  };
