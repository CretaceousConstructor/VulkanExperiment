#pragma once
#include "VkInitializer.h"
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

	void Run();
	
	
	void Init();
	void RendererSetUp();
	void RenderingPreparation();
	void MainLoop();
	void CleanUp();




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

