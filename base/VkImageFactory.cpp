#include "VkImageFactory.h"
VkImageFactory::VkImageFactory(VkGraphicsComponent& _gfx):
    gfx(_gfx),
	device_manager(gfx.DeviceMan()),
	swapchain_manager(gfx.SwapchainMan()),
	command_manager(gfx.CommandMan()),
	window(gfx.Window())
{
}


std::shared_ptr<VkImageBase> VkImageFactory::ProduceImage()
{



	result.reset();

	if (!factory_state_modified)
	{
		RestoreToDefaultState();
	}

	BuildImage();
	CreateAndBindMemory();
	BuildImageView();
	Assemble();
	TransitionImageLayout();


	RestoreToDefaultState();
	return result;


}

VkImageBundle VkImageFactory::ProduceImageBundle(uint32_t bundle_size)
{


	std::vector<std::shared_ptr<VkImageBase>> result_bundle;
	for (size_t i = 0; i < bundle_size; i++)
	{
		result_bundle.push_back(ProduceImage( ));
	}
	return {std::move(result_bundle), bundle_size};


}



