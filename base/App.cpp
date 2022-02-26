#include "App.h"

void App::Init()
{
	InitRenderer();
	//global initialization
	window.Init((void *) (renderer.get()));

	VkInitializer::CreateInstance(instance, validation_manager);

//	VkValidationManager::SetupDebugMessenger(instance, validation_manager);

	VkInitializer::CreateSurface(instance, window);

	VkInitializer::PickPhysicalDevice(instance, window.GetSurface(), device_manager.GetPhysicalDeviceRef());

	device_manager.CreateLogicalDeviceAndQueues(window.GetSurface());

	swap_chain_manager.CreateSwapChainAndSwapImages(device_manager.GetPhysicalDeviceRef(), device_manager.GetLogicalDeviceRef(), window.GetSurface(), window);
}

void App::RendererSetUp()
{
	//renderer sets some references
	renderer->SetDeviceManager(device_manager);

	renderer->SetSwapChinManager(swap_chain_manager);

	renderer->SetWindow(window);

	renderer->SetGraphicsCommandPool(
	    device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::graphics_command_pool, window.GetSurface()));
	renderer->SetTransforCommandPool(
	    device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::transfor_command_pool, window.GetSurface()));
}

void App::RenderingPreparation()
{
	renderer->SetUpUserInput();
	//Init Camera
	renderer->CreateCamera();
	//prepare command buffer
	renderer->InitCommandBuffers();
	//prepare Models
	renderer->PrepareModels();

	//prepare Images
	renderer->CreateAttachmentImages();
	renderer->CreateTextureImages();
	renderer->CreateDepthImages();
	//prepare Renderpass
	renderer->CreateRenderPass();
	//prepare Buffers
	renderer->CreateUniformBuffer();
	renderer->CreateFramebuffers();
	//prepare Descriptor Sets
	renderer->CreateDescriptorSetLayout();
	renderer->CreateDescriptorPool();
	renderer->CreateDescriptorSets();
	//prepare Pipline
	renderer->CreateGraphicsPiplineLayout();
	renderer->CreateGraphicsPipline();

	//command buffer recording
	renderer->CommandBufferRecording();
	//prepare sync objects
	renderer->InitSynObjects();
}

void App::MainLoop()
{
	float time_diff = 0.f;
	while (!glfwWindowShouldClose(window.GetWindowPtr()))
	{
		auto tStart = std::chrono::high_resolution_clock::now();
		renderer->UpdateCamera(time_diff);
		renderer->DrawFrame();
		auto tEnd = std::chrono::high_resolution_clock::now();
		time_diff = std::chrono::duration<float, std::chrono::seconds::period>(tEnd - tStart).count();

		glfwPollEvents();
	}
	vkDeviceWaitIdle(device_manager.GetLogicalDeviceRef());
}

void App::CleanUp()
{
	renderer->CleanUpSyncObjects();
	renderer->CleanUpModels();
	renderer->CleanUpPiplineAndPiplineLayout();
	renderer->CleanUpDescriptorSetLayoutAndDescriptorPool();
	renderer->CleanUpCommandBuffersAndCommandPool();
	renderer->CleanupFrameBuffers();
	//TODO:clean up uniformBuffers;
	renderer->CleanUpRenderPass();
	renderer->CleanUpImages();
	swap_chain_manager.CleanUp(device_manager.GetLogicalDeviceRef());
	device_manager.CleanUp();
	window.CleanUp(instance);
	validation_manager.CleanUp(instance);
	DestroyInstance();
}

void App::DestroyInstance()
{
	vkDestroyInstance(instance, nullptr);
}

void App::Run()
{
	Init();
	RendererSetUp();
	RenderingPreparation();
	MainLoop();
	CleanUp();
}













































