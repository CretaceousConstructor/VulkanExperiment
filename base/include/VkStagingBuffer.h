#pragma once

#include "VkBufferBase.h"

class VkStagingBuffer :public VkBufferBase
{


public:


	VkStagingBuffer(VkGraphicsComponent &gfx_, const VkBuffer buffer_, const VkDeviceMemory buffer_memory_, VkDeviceSize size_);
	~VkStagingBuffer() override = default;

	VkStagingBuffer(const VkStagingBuffer &) = delete;
	VkStagingBuffer &operator=(const VkStagingBuffer &) = delete;
	VkStagingBuffer(VkStagingBuffer &&)                 = delete;
	VkStagingBuffer &operator=(VkStagingBuffer &&) = delete;

	//void MapMemory(VkDeviceSize mapped_region_starting_offset, VkDeviceSize mapped_region_size, void const *outside_data_to_be_mapped, size_t outside_data_size, VkMemoryMapFlags flgs = 0) const;



};