#pragma once
#include "App.h"
#include "Multisubpass.h"

class MultiSubPassesApp : public App
{
  public:
	MultiSubPassesApp() = default;
	~MultiSubPassesApp() = default;


  protected:
	void InitRenderer() override;
};
