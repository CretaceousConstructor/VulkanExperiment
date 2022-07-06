#pragma once

#include "VkGraphicsComponent.h"

namespace VkBufferCI
{
template <VkBufferUsageFlags U, VkSharingMode S, VkMemoryPropertyFlags M>
struct Buffer
{
	Buffer() = default;
	VkBufferUsageFlags    usage{U};
	VkSharingMode         sharing_mode{S};
	VkMemoryPropertyFlags memory_properties{M};
};


using StagingBuffer = Buffer<VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT>;

using UniformBuffer = Buffer<VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT>;

using VertexBuffer = Buffer<VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

using IndexBuffer = Buffer<VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;















}        // namespace VkBufferCI