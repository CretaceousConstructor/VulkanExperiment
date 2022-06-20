#include "VkBufferFactory.h"

VkBufferFactory::VkBufferFactory(VkGraphicsComponent &_gfx) :
    gfx(_gfx), device_manager(gfx.DeviceMan()), window(gfx.Window()), temp_buffer(nullptr), temp_buffer_memory(nullptr)
{
}

std::shared_ptr<VkBufferBase> VkBufferFactory::ProduceBuffer(VkDeviceSize buffer_size, VkMemoryPropertyFlags properties)
{

	result.reset();
	temp_buffer_size       = buffer_size;
	temp_properties = properties;
	if (!factory_state_modified)
	{
		RestoreToDefaultState();
	}
	BuildBuffer();
	BuildMemory();
	BindBufferToMemo();
	Assemble();
	RestoreToDefaultState();
	return result;
}

VkBufferBundle VkBufferFactory::ProduceBufferBundle(VkDeviceSize buffer_size, uint32_t bundle_size, VkMemoryPropertyFlags properties)
{
	std::vector<std::shared_ptr<VkBufferBase>> result_bundle;
	for (size_t i = 0; i < bundle_size; i++)
	{
		result_bundle.push_back(ProduceBuffer(buffer_size, properties));
	}
	return {std::move(result_bundle), bundle_size};
}
