#include "VkUniformBuffer.h"

VkUniformBuffer::VkUniformBuffer(VkDeviceManager &_device_manager, VkWindows &_window, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharing_mode) :
    device_manager(_device_manager), window(_window)
{
	device_manager.CreateBuffer(size, usage, properties, buffer, memory, sharing_mode, window.GetSurfaceRef());




}

VkWriteDescriptorSet VkUniformBuffer::GetWriteDescriptorSetInfo(uint32_t dstbinding,uint32_t                         dstArrayElement   )
{
	buffer_info_write.buffer = buffer;
	buffer_info_write.offset = 0;
	buffer_info_write.range  = sizeof(Ubo_data);

	VkWriteDescriptorSet              temp_writeDescriptorSet{};
	temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_writeDescriptorSet.dstBinding      = dstbinding;
	temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	temp_writeDescriptorSet.descriptorCount = 1;
	temp_writeDescriptorSet.pBufferInfo     = &buffer_info_write;
	temp_writeDescriptorSet.dstArrayElement = dstArrayElement;

	return temp_writeDescriptorSet;
}

std::vector<std::unique_ptr<VkUniformBuffer>> VkUniformBufferFactory::GetBufferBundle(VkDeviceSize buffer_size, uint32_t bundle_size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{

	std::vector<std::unique_ptr<VkUniformBuffer>> uniform_buffers;


	for (size_t i = 0; i < bundle_size; i++)
	{
		uniform_buffers.emplace_back(std::make_unique<VkUniformBuffer>(device_manager, window, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_SHARING_MODE_EXCLUSIVE));
	}

	return uniform_buffers;




}

void VkUniformBufferFactory::SetSharingMode(VkSharingMode _sharing_mode)
{

	sharing_mode = _sharing_mode;
}
