#pragma once
#include "VkInitializer.h"
#include "VkDeviceManager.h"
#include "VkExtensionManager.h"
#include "VkSwapChainManager.h"
#include "VkWindows.h"
#include "ShadowMappingRenderer.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <memory>


class App
{

public:

	void Run();

protected:

	virtual void Init() = 0;
	virtual void RendererSetUp();
	virtual void RenderingPreparation();
	virtual void MainLoop();
	virtual void CleanUp();
	virtual void DestroyInstance();



	VkWindows window;

	VkInstance instance;

	VkValidationManager validation_manager;

	VkDeviceManager device_manager;

	VkSwapChainManager swap_chain_manager;

	//Renderer renderer;


	std::unique_ptr<BaseRenderer> renderer;










};

