#include "BaseRenderer.h"

BaseRenderer::BaseRenderer(VkGraphicsComponent &gfx_) :
    gfx(gfx_),
    device_manager(gfx.DeviceMan()),
    window(gfx.Window()),
    swapchain_manager(gfx.SwapchainMan()),
    command_manager(gfx.CommandMan())
{
}

void BaseRenderer::MaterialRegister()
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
	this->CreateCommomAttachmentImgs();
	this->CreateCommonTextureImgs();
	//Buffers
	this->CreateCommonUniformBuffer();

	//DESCRIPTOR POOL
	this->CreateCommonDescriptorPool();

	//*************MODEL INIT***************
	this->PrepareCommonModels();

	//*************RENDERPASS INIT***************
	this->InitRenderpasses();

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
