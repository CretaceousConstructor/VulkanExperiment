#pragma once

#include "App.h"
#include "KTXTextureRenderer.h"

class KTXTextureApp : public App
{
  public:
	~KTXTextureApp() = default;
	KTXTextureApp()  = default;

  protected:
	void InitRenderer() override;
};
