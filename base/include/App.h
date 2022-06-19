#pragma once
#include "VkDeviceManager.h"
#include "VkExtensionManager.h"
#include "VkSwapChainManager.h"
#include "VkWindows.h"
#include "VkInstanceWrapper.h"
#include "BaseRenderer.h"
#include "VkCommandManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <memory>


class App
{

public:
	App() ;
	virtual ~App() = default;


	App(const App &) = delete;
	App &operator=(const App&) = delete;

	App(App&&) = delete;
	App &operator=(App&&) = delete;
public:
	void Run();
	void Init();
	void MainLoop() const;



protected:
	virtual void InitRenderer() = 0;

protected:
	VkInstanceWrapper instance;
	VkWindows window;
	VkDeviceManager device_manager;
	VkSwapChainManager swap_chain_manager;
	VkCommandManager    command_manager;

	std::unique_ptr<BaseRenderer> renderer;


};

