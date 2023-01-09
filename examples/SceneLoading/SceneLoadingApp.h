#pragma once

#include "App.h"
#include "SceneLoadingRenderer.h"

class SceneLoadingApp : public App
{
  public:
	SceneLoadingApp()  = default;
	~SceneLoadingApp() override = default ;

  protected:
	void InitRenderer() override final;
};
