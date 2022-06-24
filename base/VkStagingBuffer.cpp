#include "VkStagingBuffer.h"


VkStagingBuffer::VkStagingBuffer(VkGraphicsComponent &gfx_, const VkBuffer buffer_, const VkDeviceMemory buffer_memory_, VkDeviceSize size_)
		:VkBufferBase(gfx_,buffer_,buffer_memory_,size_)
{
	
}

//void VkStagingBuffer::MapMemory(VkDeviceSize mapped_region_starting_offset, VkDeviceSize mapped_region_size, void const *outside_data_to_be_mapped, size_t outside_data_size, VkMemoryMapFlags flgs) const
//{
//
//
//	void *data;
//	vkMapMemory(device_manager.GetLogicalDevice(), buffer_memory, mapped_region_starting_offset, mapped_region_size, flgs, &data);
//	memcpy(data, outside_data_to_be_mapped, outside_data_size);
//	vkUnmapMemory(device_manager.GetLogicalDevice(), buffer_memory);
//
//
//}
//
//
//
//
