#pragma once

#include <list>

#include <mathernogl/MathernoGL.h>
#include "RenderSystem.h"

class RenderableSetImpl : public RenderableSet {
protected:
  mathernogl::MappedList<RenderablePtr> renderables;

public:
  RenderableSetImpl(uint id);

  virtual void initialise(RenderContext* renderContext) override;
  virtual void cleanUp(RenderContext* renderContext) override;
  virtual void render(RenderContext* renderContext) override;
  virtual void renderShadowMap(RenderContext* renderContext) override;
  virtual void addRenderable(RenderablePtr renderable) override;
  virtual void forEachChild(std::function<void(RenderablePtr)> function) override;
  virtual void removeRenderable(int id) override;
  virtual bool containsRenderable(int id) const override;
  virtual int count() const override;

protected:
  void render(RenderContext* renderContext, bool shadowMap);
  };
