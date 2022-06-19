#include "BaseRenderer.h"

BaseRenderer::BaseRenderer(VkWindows &_window, VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkCommandManager &_command_manager)
    :
    device_manager(_device_manager),
    window(_window),
    swapchain_manager(_swapchain_manager),
    command_manager(_command_manager)
{


}

void BaseRenderer::RenderingPreparation()
{

	this->SetUpUserInput();
	//Init Camera
	this->CreateCamera();
	//prepare Models
	this->PrepareModels();


	//prepare Images
	this->CreateAttachmentImages();
	this->CreateTextureImages();
	this->CreateDepthImages();

	//prepare Buffers
	this->CreateUniformBuffer();


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


//BaseRenderer *BaseRenderer::GetThisPtr()
//{
//	return this;
//}
