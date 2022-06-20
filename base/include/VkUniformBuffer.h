#pragma once

#include "VkGraphicsComponent.h"
#include "VkBufferBase.h"
#include <vector>



//TODO:把cpu端的数据整合到这个类里面
class VkUniformBuffer :public VkBufferBase
{
public:

	VkUniformBuffer(VkGraphicsComponent &_gfx, const VkBuffer _buffer, const VkDeviceMemory _buffer_memory, VkDeviceSize _size);

	~VkUniformBuffer() override = default;
	VkUniformBuffer(const VkUniformBuffer &) = delete;
	VkUniformBuffer &operator=(const VkUniformBuffer &) = delete;
	VkUniformBuffer(VkUniformBuffer &&) = delete;
	VkUniformBuffer &operator=(VkUniformBuffer&&) = delete;

	void MapMemory(VkDeviceSize mapped_region_starting_offset, VkDeviceSize mapped_region_size, void const *outside_data_to_be_mapped, size_t outside_data_size, VkMemoryMapFlags flgs = 0) const;

};
