#pragma once

#include <SMGameActor.h>
#include <SMGameObjectFactory.h>

/*
*   
*/


class ComponentVoxelModelBlueprint : public SMComponentBlueprint
  {
public:
  std::vector<string> modelFilePaths;
  bool staticModel;

public:
  virtual bool loadFromXML(XMLElement* xmlComponent, string* errorMsg) override;
  virtual SMComponentPtr constructComponent(SMGameActorPtr actor) const override;
  };



class ComponentVoxelModel : public SMComponent
  {
private:
  const ComponentVoxelModelBlueprint* blueprint;
  RenderablePtr voxelModel;
  uint voxelModelIdx;

public:
  ComponentVoxelModel(const SMGameActorPtr& actor, SMComponentType type, const ComponentVoxelModelBlueprint* blueprint);

  virtual void initialise(GameContext* gameContext) override;
  virtual void initialiseFromSaved(GameContext* gameContext, XMLElement* xmlComponent) override;
  virtual void update(GameContext* gameContext) override;
  virtual void cleanUp(GameContext* gameContext) override;
  virtual void onMessage(GameContext* gameContext, SMMessage message, void* extra) override;
  virtual void save(GameContext* gameContext, XMLElement* xmlComponent) override;

protected:
  void createRenderable(GameContext* gameContext, const string& filePath);
  void updateRenderableTransform();
  };
