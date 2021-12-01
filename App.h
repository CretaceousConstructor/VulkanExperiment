#pragma once
#include "VkInitializer.h"
#include "VkDeviceManager.h"
#include "VkExtensionManager.h"
#include "VkSwapChainManager.h"
#include "VkWindows.h"
#include "Renderer.h"
class App
{

public:

	void Run();

private:

	void Init();
	void RendererSetUp();
	void RenderingPreparation();
	void MainLoop();
	void CleanUp();
	void DestroyInstance();



	VkWindows window;

	VkInstance instance;

	VkValidationManager validation_manager;

	VkDeviceManager device_manager;

	VkSwapChainManager swap_chain_manager;

	Renderer renderer;
















};

