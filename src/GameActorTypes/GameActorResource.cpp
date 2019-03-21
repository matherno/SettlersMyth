//
// Created by matt on 27/02/19.
//

#include "GameActorResource.h"
#include "BlueprintFileHelper.h"


bool GameActorResourceBlueprint::loadFromXML(XMLElement* xmlElement, string* errorMsg)
  {
  if(!SMGameActorBlueprint::loadFromXML(xmlElement, errorMsg))
    return false;

  stackPosAndRots.clear();
  auto xmlStackPos = xmlElement->FirstChildElement(OD_STACKPOS);
  while (xmlStackPos)
    {
    const double x = xmlStackPos->DoubleAttribute(OD_X, 0);
    const double y = xmlStackPos->DoubleAttribute(OD_ELEV, 0);
    const double z = -xmlStackPos->DoubleAttribute(OD_Y, 0);
    const double rot = xmlStackPos->DoubleAttribute(OD_ROT, 0);
    stackPosAndRots.emplace_back(Vector3D(x, y, z), rot);
    xmlStackPos = xmlStackPos->NextSiblingElement(OD_STACKPOS);
    }

  return true;
  }

SMGameActor* GameActorResourceBlueprint::doConstructActor(uint actorID) const
  {
  return new GameActorResource(actorID, id, this);
  }



GameActorResource::GameActorResource(uint id, uint typeID, const GameActorResourceBlueprint* blueprint) 
      : SMGameActor(id, typeID), blueprint(blueprint)
  {
  }