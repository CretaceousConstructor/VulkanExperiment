#include "VkBufferBase.h"

VkBufferBase::VkBufferBase(VkGraphicsComponent &_gfx, const VkBuffer _buffer, const VkDeviceMemory _buffer_memory,VkDeviceSize _size):
	gfx(_gfx),device_manager(gfx.DeviceMan()),buffer(_buffer),buffer_memory(_buffer_memory),size_of_buffer(_size),mapped(nullptr)
{

}



VkBufferBase::~VkBufferBase()
{
	vkDestroyBuffer(device_manager.GetLogicalDevice(), buffer, nullptr);
	vkFreeMemory(device_manager.GetLogicalDevice(), buffer_memory, nullptr);
}

VkWriteDescriptorSet VkBufferBase::GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement)
{
	
	buffer_des_info_write.buffer = buffer;
	buffer_des_info_write.offset = 0;
	buffer_des_info_write.range  = size_of_buffer;

	VkWriteDescriptorSet temp_writeDescriptorSet{};
	temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_writeDescriptorSet.dstBinding      = dstbinding;
	temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	temp_writeDescriptorSet.descriptorCount = 1;
	temp_writeDescriptorSet.pBufferInfo     = &buffer_des_info_write;
	temp_writeDescriptorSet.dstArrayElement = dstArrayElement;

	return temp_writeDescriptorSet;

}



