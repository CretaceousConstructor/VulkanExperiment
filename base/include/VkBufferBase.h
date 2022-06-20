#pragma once
#include "VkGraphicsComponent.h"


class VkBufferBase
{
  protected:
	VkBufferBase(VkGraphicsComponent &_gfx, const VkBuffer _buffer, const VkDeviceMemory _buffer_memory, VkDeviceSize _size);
  public:
	virtual ~VkBufferBase() = 0;

	VkBufferBase()                        = delete;
	VkBufferBase(const VkBufferBase &) = delete;
	VkBufferBase &operator=(const VkBufferBase &) = delete;

	VkBufferBase(VkBufferBase &&) = delete;
	VkBufferBase &operator=(VkBufferBase &&) = delete;

	VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement);

  protected:
	VkGraphicsComponent &gfx;
	const VkDeviceManager& device_manager;

  protected:
	VkBuffer               buffer{};
	VkDeviceMemory         buffer_memory{};
	VkDeviceSize           size_of_buffer{};
	VkDescriptorBufferInfo buffer_des_info_write{};

	//const void * const mapped{};
};
