#pragma once
#include "App.h"
#include "DeferedRenderingRenderer.h"

class DeferedRenderingApp: public App
{
  public:
	DeferedRenderingApp() = default;
	~DeferedRenderingApp()  = default;

  protected:
	void InitRenderer() override;
};


