#include "VkBufferBase.h"

VkBufferBase::VkBufferBase(VkGraphicsComponent &gfx_, const VkBuffer buffer_, const VkDeviceMemory buffer_memory_, VkDeviceSize size_,VkDeviceSize mem_required_size_) :
    gfx(gfx_), device_manager(gfx.DeviceMan()), buffer(buffer_), buffer_memory(buffer_memory_), size_of_buffer(size_),mem_required_size(mem_required_size_)
{


}

VkBufferBase::~VkBufferBase()
{
	Unmap();
	vkDestroyBuffer(device_manager.GetLogicalDevice(), buffer, nullptr);
	vkFreeMemory(device_manager.GetLogicalDevice(), buffer_memory, nullptr);


}

VkWriteDescriptorSet VkBufferBase::GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement, VkDeviceSize size, VkDeviceSize offset)
{
	buffer_des_info_write.buffer = buffer;
	buffer_des_info_write.offset = offset;
	buffer_des_info_write.range  = size;

	VkWriteDescriptorSet temp_writeDescriptorSet{};
	temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_writeDescriptorSet.dstBinding      = dstbinding;
	temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	temp_writeDescriptorSet.descriptorCount = 1;
	temp_writeDescriptorSet.pBufferInfo     = &buffer_des_info_write;
	temp_writeDescriptorSet.dstArrayElement = dstArrayElement;

	return temp_writeDescriptorSet;
}

void VkBufferBase::CopyFrom(void const *outside_data_to_be_mapped, size_t outside_data_size, VkDeviceSize mapped_region_starting_offset) const
{
	if (nullptr==mapped_ptr_to_GPU_memory)
	{
		throw std::runtime_error("No mapped region found!");
	}

	memcpy(static_cast<void *>((static_cast<uint8_t *>(mapped_ptr_to_GPU_memory) + mapped_region_starting_offset)), outside_data_to_be_mapped, outside_data_size);
}

void VkBufferBase::MapMemory(VkDeviceSize size, VkDeviceSize offset)
{
	VK_CHECK_RESULT(vkMapMemory(device_manager.GetLogicalDevice(), buffer_memory, offset, size, 0, &mapped_ptr_to_GPU_memory))
}

void VkBufferBase::Flush(VkDeviceSize size, VkDeviceSize offset_in_whole_mem) const
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory              = buffer_memory;
	mappedRange.offset              = offset_in_whole_mem;
	mappedRange.size                = size;

	VK_CHECK_RESULT(vkFlushMappedMemoryRanges(device_manager.GetLogicalDevice(), 1, &mappedRange))
}

void VkBufferBase::Unmap()
{
	if (mapped_ptr_to_GPU_memory)
	{
		(vkUnmapMemory(device_manager.GetLogicalDevice(), buffer_memory));
		mapped_ptr_to_GPU_memory = nullptr;
	}
}

void VkBufferBase::Invalidate(VkDeviceSize size, VkDeviceSize offset_in_whole_mem) const
{
	VkMappedMemoryRange mappedRange = {};
	mappedRange.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory              = buffer_memory;
	mappedRange.offset              = offset_in_whole_mem;
	mappedRange.size                = size;
	VK_CHECK_RESULT(vkInvalidateMappedMemoryRanges(device_manager.GetLogicalDevice(), 1, &mappedRange))

}

void *VkBufferBase::GetPtrToMappedRegion() const
{
	return mapped_ptr_to_GPU_memory;
}

const VkBuffer &VkBufferBase::GetGPUBuffer() const

{
	return buffer;
}
