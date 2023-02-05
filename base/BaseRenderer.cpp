#include "BaseRenderer.h"

BaseRenderer::BaseRenderer(VkGraphicsComponent &gfx_) :
    gfx(gfx_),
    device_manager(gfx.DeviceMan()),
    window(gfx.Window()),
    swapchain_manager(gfx.SwapchainMan()),
    command_manager(gfx.CommandMan())
{
}

BaseRenderer::~BaseRenderer()
{
	MaterialUnRegistration();
}

void BaseRenderer::MaterialRegistration() const
{
	VkMaterial::Register<PbrMaterialMetallic>(gfx);
	VkMaterial::Register<NonPbrMaterial>(gfx);
}

void BaseRenderer::MaterialUnRegistration() const
{
	VkMaterial::UnRegister<PbrMaterialMetallic>(gfx);
	VkMaterial::UnRegister<NonPbrMaterial>(gfx);
}

void BaseRenderer::RenderingPreparation()
{
	MaterialRegistration();
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
