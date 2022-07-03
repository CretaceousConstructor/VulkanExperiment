#pragma once

#include "VkBufferBase.h"
#include "VkGraphicsComponent.h"
#include <vector>

////TODO:把cpu端的数据整合到这个类里面
//class VkStagingBuffer : public VkBufferBase
//{
//  public:
//	VkStagingBuffer(VkGraphicsComponent &_gfx, const VkBuffer _buffer, const VkDeviceMemory _buffer_memory, VkDeviceSize _size);
//
//	~VkStagingBuffer() override              = default;
//	VkStagingBuffer(const VkStagingBuffer &) = delete;
//	VkStagingBuffer &operator=(const VkStagingBuffer &) = delete;
//	VkStagingBuffer(VkStagingBuffer &&)                 = delete;
//	VkStagingBuffer &operator=(VkStagingBuffer &&) = delete;
//
//};

