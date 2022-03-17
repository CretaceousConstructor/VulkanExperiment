#pragma once
#include "VkInitializer.h"
#include "VkDeviceManager.h"
#include "VkExtensionManager.h"
#include "VkSwapChainManager.h"
#include "VkWindows.h"
#include "VkInstanceWrapper.h"
#include "BaseRenderer.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <memory>


class App
{

public:
	App();
	void Run();
	~App() = default;

	void Init();
	void RendererSetUp();
	void RenderingPreparation();
	void MainLoop();
	void CleanUp();
	void DestroyInstance();


protected:
	virtual void InitRenderer() = 0;


	VkWindows window;

	VkInstance instance;

	VkValidationManager validation_manager;


	VkDeviceManager device_manager;
	VkSwapChainManager swap_chain_manager;
	std::unique_ptr<BaseRenderer> renderer;
	
	
	
	
	
	//std::shared_ptr<VkWindows> window_v1;
	//std::shared_ptr<VkInstanceWrapper> instance_v1;




};

