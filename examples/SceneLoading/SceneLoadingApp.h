#pragma once

#include "App.h"
#include "SceneLoadingRenderer.h"

class SceneLoadingApp : public App
{
  public:
	SceneLoadingApp()  = default;
	~SceneLoadingApp() = default;

  protected:
	void InitRenderer() override;
};
