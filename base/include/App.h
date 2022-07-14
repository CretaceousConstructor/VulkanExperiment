#pragma once
#include "BaseRenderer.h"
#include "VkGraphicsComponent.h"
#include "VkInstanceWrapper.h"

#include "VkMath.h"

#include <memory>

class App
{
  public:
	App();
	virtual ~App() = default;

	App(const App &) = delete;
	App &operator=(const App &) = delete;

	App(App &&)  = delete;
	App &operator=(App &&) = delete;

  public:
	void Run();
	void MainLoop() const;


  protected:
	virtual void InitRenderer() = 0;

  protected:
	VkInstanceWrapper  instance;
	VkWindows          window;
	VkDeviceManager    device_manager;
	VkSwapchainManager swap_chain_manager;
	VkCommandManager   command_manager;

	VkGraphicsComponent gfx;

	std::unique_ptr<BaseRenderer> renderer;



};
