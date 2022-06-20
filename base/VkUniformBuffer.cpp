#include "VkUniformBuffer.h"


VkUniformBuffer::VkUniformBuffer(VkGraphicsComponent &_gfx, const VkBuffer _buffer, const VkDeviceMemory _buffer_memory, VkDeviceSize _size) :
    VkBufferBase(_gfx, _buffer, _buffer_memory, _size)
{
}


void VkUniformBuffer::MapMemory(VkDeviceSize mapped_region_starting_offset, VkDeviceSize mapped_region_size, void const *outside_data_to_be_mapped, size_t outside_data_size, VkMemoryMapFlags flgs) const
{
	void *data;
	vkMapMemory(device_manager.GetLogicalDevice(), buffer_memory, mapped_region_starting_offset, mapped_region_size, flgs, &data);
	memcpy(data, outside_data_to_be_mapped, outside_data_size);
	vkUnmapMemory(device_manager.GetLogicalDevice(), buffer_memory);

}
