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
	//***************USER INPUT***************
	this->SetUpUserInput();
	//Init Camera
	this->CreateCamera();

	//**************COMMON RESOURCES***************
	//Images
	this->CreateAttachmentImages();
	this->CreateTextureImages();
	//Buffers
	this->CreateUniformBuffer();
	//descriptor pool
	this->CreateDescriptorPool();
	//Models
	this->PrepareModels();

	//*************RENDERPASS INIT***************
	this->RenderpassInit();

	//*************COMMAND BUFFER RECORDING***************
	//prepare sync objects
	this->InitSynObjects();
	//command buffer recording
	this->CommandBufferRecording();



}


//BaseRenderer *BaseRenderer::GetThisPtr()
//{
//	return this;
//}
