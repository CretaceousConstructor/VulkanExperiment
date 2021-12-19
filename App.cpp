#include "App.h"

void App::Init()
{
	renderer = std::make_unique<ShadowMapping>();

	//global initialization
	window.Init((void*)(renderer.get()));

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
	renderer->SetDeviceManager(device_manager);

	renderer->SetSwapChinManager(swap_chain_manager);

	renderer->SetWindow(window);

	renderer->SetGraphicsCommandPool(
		device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::graphics_command_pool, window.GetSurface())
	);
	renderer->SetTransforCommandPool(
		device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::transfor_command_pool, window.GetSurface())
	);


}

void App::RenderingPreparation()
{

	renderer->SetUpUserInput();
	//Init Camera
	renderer->CreateCamera();
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
	//prepare command buffer
	renderer->InitCommandBuffers();
	//prepare Models
	renderer->PrepareModels();
	//command buffer recording
	renderer->CommandBufferRecording();
	//prepare sync objects
	renderer->InitSynObjects();




}

void App::MainLoop()
{




	//////glm::yawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//auto R = glm::yawPitchRoll(0.7853981633974483, 0., 0.);



	//////auto R = glm::translate(glm::mat4(1.0f), glm::vec3(1.,2.,3.));


	//glm::vec4 some = { 1,0,0,1 };

	//some = R * some ;


	////////for (int i = 0; i < 4; i++) {

	////////	for (int j = 0; j < 4; j++) {
	////////		std::cout << R[i][j] << "            ";

	////////	}
	////////	std::cout << std::endl;
	////////}


	//for (int i = 0; i < 4; i++) {
	//	std::cout << some[i] << "    ";
	//}

	//std::cout << std::endl;






	float time_diff = 0.f;
	while (!glfwWindowShouldClose(window.GetWindowPtr())) {


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


