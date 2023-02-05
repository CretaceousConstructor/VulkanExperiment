#pragma once

#include "App.h"
#include "Renderer.h"

class GraphicsApp : public App
{
  public:
	GraphicsApp()  = default;
	~GraphicsApp() override = default ;

  protected:
	void InitRenderer() override final;
};
