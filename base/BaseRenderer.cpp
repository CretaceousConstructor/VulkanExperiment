#include "BaseRenderer.h"

BaseRenderer::BaseRenderer(VkWindows &_window, VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkCommandManager &_command_manager)

    :
    swapchain_manager(_swapchain_manager),
    device_manager(_device_manager),
    window(_window),
    command_manager(_command_manager)
{



}

void BaseRenderer::RenderingPreparation()
{

	//TODO:做成模板方法
	this->SetUpUserInput();
	//Init Camera
	this->CreateCamera();
	//prepare command buffer
	this->InitCommandBuffers();
	//prepare Models
	this->PrepareModels();


	//prepare Images
	this->CreateAttachmentImages();
	this->CreateTextureImages();
	this->CreateDepthImages();

	//prepare Buffers
	this->CreateUniformBuffer();
	this->CreateFrameBuffers();


	//prepare Descriptor Sets
	this->CreateDescriptorSetLayout();
	this->CreateDescriptorPool();
	this->CreateDescriptorSets();


	//prepare Renderpass
	this->CreateRenderPass();
	//prepare Pipeline
	//this->CreateGraphicsPipelineLayout();
	this->CompileShaders();
	this->CreateGraphicsPipeline();

	//command buffer recording
	this->CommandBufferRecording();
	//prepare sync objects
	this->InitSynObjects();



}

//void BaseRenderer::SetDeviceManager(VkDeviceManager &para_device_manager)
//{
//	device_manager = &para_device_manager;
//}
//void BaseRenderer::SetSwapChinManager(VkSwapChainManager &para_swapchain_manager)
//{
//	swapchain_manager = &para_swapchain_manager;
//}
//
//void BaseRenderer::SetCommandManager(VkCommandManager &_commandman)
//{
//
//
//	command_manager= &_commandman;
//}
//
//void BaseRenderer::SetWindow(VkWindows &para_window)
//{
//	window = &para_window;
//}
//
//void BaseRenderer::SetGraphicsCommandPool(VkCommandPool commandpool)
//{
//	command_manager->graphics_command_pool = commandpool;
//	graphics_command_pool                  = commandpool;
//	return;
//}
//
//void BaseRenderer::SetTransforCommandPool(VkCommandPool commandpool)
//{
//	command_manager->transfer_command_pool = commandpool;
//	transfer_command_pool = commandpool;
//	return;
//}
//
//BaseRenderer *BaseRenderer::GetThisPtr()
//{
//	return this;
//}
