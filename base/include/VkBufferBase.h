#pragma once
#include "VkGraphicsComponent.h"

class VkBufferBase
{
  public:
	VkBufferBase(VkGraphicsComponent &_gfx, const VkBuffer _buffer, const VkDeviceMemory _buffer_memory, VkDeviceSize _size);

  public:
	~VkBufferBase();

	VkBufferBase()                     = delete;
	VkBufferBase(const VkBufferBase &) = delete;
	VkBufferBase &operator=(const VkBufferBase &) = delete;
	VkBufferBase(VkBufferBase &&)                 = delete;
	VkBufferBase &operator=(VkBufferBase &&) = delete;

	VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement = 0);
	void                 MapMemory(VkDeviceSize mapped_region_starting_offset, VkDeviceSize mapped_region_size, void const *outside_data_to_be_mapped, size_t outside_data_size, VkMemoryMapFlags flgs = 0) const;
	[[nodiscard]] const VkBuffer&             GetBuffer() const;




  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  private:
	VkBuffer               buffer{};
	VkDeviceMemory         buffer_memory{};
	VkDeviceSize           size_of_buffer{};
	VkDescriptorBufferInfo buffer_des_info_write{};
};
