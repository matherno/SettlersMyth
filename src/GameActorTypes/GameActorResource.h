#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>

/*
 *
 */


class GameActorResourceBlueprint : public SMGameActorBlueprint
  {
public:
  std::vector<std::pair<Vector3D, double>> stackPosAndRots;

protected:
  virtual bool loadFromXML(XMLElement* xmlElement, string* errorMsg) override;
  virtual SMGameActor *doConstructActor(uint actorID) const override;

public:
  static const GameActorResourceBlueprint* cast(const SMGameActorBlueprint* blueprint) { return dynamic_cast<const GameActorResourceBlueprint*>(blueprint); }   
  };

class GameActorResource : public SMGameActor
  {
private:
  const GameActorResourceBlueprint* blueprint;

public:
  GameActorResource(uint id, uint typeID, const GameActorResourceBlueprint* blueprint);

  static GameActorResource* cast(SMGameActor* actor) { return dynamic_cast<GameActorResource*>(actor); }   
  };
