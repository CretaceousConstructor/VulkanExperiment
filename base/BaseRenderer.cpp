#include "BaseRenderer.h"

BaseRenderer::BaseRenderer(VkGraphicsComponent& gfx_)
    :
    gfx(gfx_),
    device_manager(gfx.DeviceMan()),
    window(gfx.Window()),
    swapchain_manager(gfx.SwapchainMan()),
    command_manager(gfx.CommandMan())
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
