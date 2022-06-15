#include "VkUniformBuffer.h"

VkUniformBuffer::VkUniformBuffer(VkDeviceManager &_device_manager, VkWindows &_window, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharing_mode) :
    device_manager(_device_manager), window(_window)
{

	device_manager.CreateBufferAndBindToMemo(size, usage, properties, buffer, memory, sharing_mode, window.GetSurface());

}

VkUniformBuffer::~VkUniformBuffer()
{
	vkFreeMemory(device_manager.GetLogicalDevice(), memory, nullptr);
	vkDestroyBuffer(device_manager.GetLogicalDevice(), buffer, nullptr);
}

VkWriteDescriptorSet VkUniformBuffer::GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement) 
{


	buffer_info_write.buffer = buffer;
	buffer_info_write.offset = 0;
	buffer_info_write.range  = sizeof(Ubo_data);

	VkWriteDescriptorSet temp_writeDescriptorSet{};
	temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_writeDescriptorSet.dstBinding      = dstbinding;
	temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	temp_writeDescriptorSet.descriptorCount = 1;
	temp_writeDescriptorSet.pBufferInfo     = &buffer_info_write;
	temp_writeDescriptorSet.dstArrayElement = dstArrayElement;

	return temp_writeDescriptorSet;
}

void VkUniformBuffer::MapMemory(VkDeviceSize mapped_region_starting_offset, VkDeviceSize mapped_region_size, void const *outside_data_to_be_mapped, size_t outside_data_size, VkMemoryMapFlags flgs) const
	{
			void *data;
			vkMapMemory(device_manager.GetLogicalDevice(), memory, mapped_region_starting_offset, mapped_region_size, flgs, &data);

			memcpy(data, outside_data_to_be_mapped, outside_data_size);

			vkUnmapMemory(device_manager.GetLogicalDevice(), memory);

	}


