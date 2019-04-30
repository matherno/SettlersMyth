#pragma once

#include <GameSystem/GameSystem.h>
#include <GameSystem/Timer.h>
#include "Grid.h"
#include "SaveLoadFileHelper.h"
#include "ResourceStorage.h"
#include "Utils.h"


/*
*   
*/


class UIPanel;
class SMGameActor;
typedef std::shared_ptr<SMGameActor> SMGameActorPtr;
typedef std::weak_ptr<SMGameActor> SMGameActorWkPtr;

class SMComponent
  {
private:
  SMGameActorPtr actor;
  const SMComponentType type;
  uint id = 0;

public:
  SMComponent(SMGameActorPtr actor, SMComponentType type) : actor(actor), type(type){};
  SMGameActorPtr getActor() const { return actor; }
  virtual SMComponentType getType() const { return type; }
  virtual void setComponentID(uint id) { this->id = id; }
  virtual uint getComponentID() const { return id; }
  virtual void initialise(GameContext* gameContext) {};
  virtual void initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent) { initialise(gameContext); }
  virtual void update(GameContext* gameContext) {};
  virtual void cleanUp(GameContext* gameContext) {};
  virtual void onMessage(GameContext* gameContext, SMMessage message, void* extra = nullptr) {};
  virtual void save(GameContext* gameContext, XMLElement* xmlComponent) {};

  //  onSetupSelectionHUD returns the height in pixels taken up by any created UI
  virtual int onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset) { return 0; } 
  virtual void onUpdateSelectionHUD(GameContext* gameContext) {}
  };
typedef std::shared_ptr<SMComponent> SMComponentPtr;



class SMGameActor : public GameActor, public ResourceStorage
  {
private:
  uint smActorLinkID = 0;
  std::vector<SMComponentPtr> components;
  const uint blueprintID;
  XMLElement* xmlToLoadFrom = nullptr;
  bool savingEnabled = true;
  bool initialised = false;
  string name;
  bool isPosInCentre = false;
  Vector3D position;
  Vector3D size;
  double rotation = 0;
  bool positionChanged = false;
  bool rotationChanged = false;
  bool isSelectable = false;
  UIComponentPtr selectionHUD;

protected:
  uint boundingBoxID = 0;
  BoundingBoxPtr boundingBox;

public:
  SMGameActor(uint id, uint blueprintID);
  virtual void onAttached(GameContext* gameContext) override;
  virtual void onUpdate(GameContext* gameContext) override;
  virtual void onDetached(GameContext* gameContext) override;
  virtual void saveActor(XMLElement* element, GameContext* gameContext);

  void setLinkID(uint linkID) { smActorLinkID = linkID; }
  uint getLinkID() const { return smActorLinkID; }
  void addComponent(SMComponentPtr component);
  void setTypeName(string name) { this->name = name; }
  string getTypeName() const { return name; }
  uint getBlueprintTypeID() const { return blueprintID; }

  void setSelectable(bool selectable);
  void setIsPosInCentre(bool isPosInCentre);
  void setHeight(double height);
  void setSize2D(Vector2D size);
  void setPosition(GridXY position);
  void setPosition(Vector2D position);
  void setPosition(Vector3D position);
  void setRotation(double rotation);
  void setElevation(double elevation);

  GridXY getGridPosition() const;
  Vector2D getPosition() const;
  Vector3D getPosition3D() const;
  Vector2D getMidPosition() const;
  Vector2D getSize() const;
  double getRotation() const;
  double getHeight() const;
  bool getIsPosInCentre() const;
  bool getIsSelectable() const;
  void setXMLToLoadFrom(XMLElement* xmlElement){ xmlToLoadFrom = xmlElement; }
  void finaliseLoading(GameContext* gameContext) { finaliseActorFromSave(gameContext, xmlToLoadFrom); };
  void dropAllResources(GameContext* gameContext, Vector2D position);
  void dropAllResources(GameContext* gameContext);
  BoundingBoxPtr getBoundingBox() { return boundingBox; }
  void setAllowSaving(bool allowSaving) { savingEnabled = allowSaving; }
  bool isSavingAllowed() const { return savingEnabled; }
  void postMessage(GameContext* gameContext, SMMessage message, void* extra = nullptr);
  bool gotComponentType(SMComponentType type) const;

  void setupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel);
  void updateSelectionHUD(GameContext* gameContext);
  void removeSelectionHUD(GameContext* gameContext, UIPanel* parentPanel);

protected:
  virtual void initialiseActorFromSave(GameContext* gameContext, XMLElement* element);
  virtual void finaliseActorFromSave(GameContext* gameContext, XMLElement* element) {};     //  called after all loaded actors are initialised
  void sendMessageToComponents(GameContext* gameContext, SMMessage message, void* extra = nullptr);
  void updateBoundingBox();

  //  following allows sub-classes to setup/update any HUD elements
  //  onSetupSelectionHUD returns the height in pixels taken up by any created UI
  virtual int onSetupSelectionHUD(GameContext* gameContext, UIPanel* parentPanel, int yOffset) { return 0; } 
  virtual void onUpdateSelectionHUD(GameContext* gameContext) {}
  };


