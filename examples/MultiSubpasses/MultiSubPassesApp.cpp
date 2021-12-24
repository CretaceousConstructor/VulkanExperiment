#include "MultiSubPassesApp.h"

void MultiSubPassesApp::Init()
{


	renderer = std::make_unique<MultiSubpassesRenderer>();

	//global initialization
	window.Init((void*)(renderer.get()));

	VkInitializer::CreateInstance(instance, validation_manager);

	VkValidationManager::SetupDebugMessenger(instance, validation_manager);

	VkInitializer::CreateSurface(instance, window);

	VkInitializer::PickPhysicalDevice(instance, window.GetSurface(), device_manager.GetPhysicalDeviceRef());

	device_manager.CreateLogicalDeviceAndQueues(window.GetSurface());

	swap_chain_manager.CreateSwapChainAndSwapImages(device_manager.GetPhysicalDeviceRef(), device_manager.GetLogicalDeviceRef(), window.GetSurface(), window);



}
