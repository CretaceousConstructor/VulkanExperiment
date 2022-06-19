#pragma once
#include "App.h"
#include "Multisubpass.h"

class MultiSubPassesApp : public App
{
  public:
	MultiSubPassesApp() = default;
	~MultiSubPassesApp() override =default;


	MultiSubPassesApp(const MultiSubPassesApp &) = delete;
	MultiSubPassesApp &operator=(const MultiSubPassesApp&) = delete;

	MultiSubPassesApp(MultiSubPassesApp&&) = delete;
	MultiSubPassesApp &operator=(MultiSubPassesApp&&) = delete;


  protected:
	void InitRenderer() override;
};
