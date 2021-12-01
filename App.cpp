#include "App.h"

void App::Init()
{


	//global initialization
	window.Init();

	VkInitializer::CreateInstance(instance, validation_manager);

	VkValidationManager::SetupDebugMessenger(instance, validation_manager);

	VkInitializer::CreateSurface(instance, window);

	VkInitializer::PickPhysicalDevice(instance, window.GetSurface(), device_manager.GetPhysicalDeviceRef());

	device_manager.CreateLogicalDeviceAndQueues(window.GetSurface());

	swap_chain_manager.CreateSwapChainAndSwapImages(device_manager.GetPhysicalDeviceRef(), device_manager.GetLogicalDeviceRef(), window.GetSurface(), window);





}

void App::RendererSetUp()
{


	//renderer sets some references
	renderer.SetDeviceManager(device_manager);

	renderer.SetSwapChinManager(swap_chain_manager);

	renderer.SetWindow(window);

	renderer.SetGraphicsCommandPool(
		device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::graphics_command_pool, window.GetSurface())
	);
	renderer.SetTransforCommandPool(
		device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::transfor_command_pool, window.GetSurface())
	);



}

void App::RenderingPreparation()
{



	//prepare Images
	renderer.CreateAttachmentImages();
	renderer.CreateTextureImages();
	renderer.CreateDepthImages();
	//prepare Renderpass
	renderer.CreateRenderPass();
	//prepare Buffers
	renderer.CreateUniformBuffer();
	renderer.CreateFramebuffers();
	//prepare Descriptor Sets
	renderer.CreateDescriptorSetLayout();
	renderer.CreateDescriptorPool();
	renderer.CreateDescriptorSets();
	//prepare Pipline
	renderer.CreateGraphicsPiplineLayout();
	renderer.CreateGraphicsPipline();
	//prepare command buffer
	renderer.InitCommandBuffers();
	//prepare Models
	renderer.PrepareModels();
	//command buffer recording
	renderer.CommandBufferRecording();
	//prepare sync objects
	renderer.InitSynObjects();

	


}

void App::MainLoop()
{

	while (!glfwWindowShouldClose(window.GetWindowPtr())) {
		glfwPollEvents();
		renderer.DrawFrame();
	}

	vkDeviceWaitIdle(device_manager.GetLogicalDeviceRef());



}

void App::CleanUp()
{

	renderer.CleanUpSyncObjects();
	//clean up modles index buffer ,vertex buffer etc.
	renderer.CleanUpModels();
	renderer.CleanUpPiplineAndPiplineLayout();
	renderer.CleanUpRenderPass();
	renderer.CleanUpDescriptorSetLayoutAndDescriptorPool();
	renderer.CleanUpCommandBuffersAndCommandPool();
	renderer.CleanupFrameBuffers();
	//clean up uniformBuffers;
	renderer.CleanUpImages();
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


