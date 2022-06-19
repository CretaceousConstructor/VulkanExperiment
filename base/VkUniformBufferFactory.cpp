#include "VkUniformBufferFactory.h"

void VkUniformBufferFactory::GetToInitalState()
{}



std::shared_ptr<VkUniformBufferBundle> VkUniformBufferFactory::GetBufferBundle(VkDeviceSize buffer_size, uint32_t bundle_size,  VkMemoryPropertyFlags properties) 
{
	std::vector<std::unique_ptr<VkUniformBuffer>> uniform_buffers;
	//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

	for (size_t i = 0; i < bundle_size; i++)
	{
		uniform_buffers.emplace_back(std::make_unique<VkUniformBuffer>(device_manager, window, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, properties, VkUniformBufferFactory::sharing_mode));
	}

	std::shared_ptr<VkUniformBufferBundle> result = std::make_shared<VkUniformBufferBundle>(std::move(uniform_buffers), bundle_size);

	GetToInitalState();

	return result;




}



void VkUniformBufferFactory::SetSharingMode(VkSharingMode _sharing_mode)
{
	sharing_mode = _sharing_mode;
}
