#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>

/*
 *
 */


class GameActorDepositBlueprint : public SMGameActorBlueprint
  {
public:
  string resourceName;
  uint resourceBlueprintID = 0;
  uint resourceAmount = 0;

  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual bool finaliseLoading(GameContext* gameContext, string* errorMsg) override;

protected:
  virtual SMGameActor *doConstructActor(uint actorID) const override;

public:
  static const GameActorDepositBlueprint* cast(const SMGameActorBlueprint* blueprint) { return dynamic_cast<const GameActorDepositBlueprint*>(blueprint); }   
  };

class GameActorDeposit : public SMGameActor
  {
private:
  friend class GameActorDepositBlueprint;
  uint resourceBlueprintID = 0;

public:
  GameActorDeposit(uint id, uint typeID);

  int depositResourceCount() const;
  int getDepositResourceBlueprintID() const;

  static GameActorDeposit* cast(SMGameActor* actor) { return dynamic_cast<GameActorDeposit*>(actor); }   
  };
  
