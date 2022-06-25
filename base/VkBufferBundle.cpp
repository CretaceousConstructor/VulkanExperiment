#include "VkBufferBundle.h"
VkBufferBundle::VkBufferBundle(std::vector<std::shared_ptr<VkBufferBase>> _uniform_buffers, uint32_t bundle_size)
    :
    bundle_count(bundle_size),
    buffers(std::move(_uniform_buffers))
{
	
}

uint32_t VkBufferBundle::GetBundleCount() const
{
	return bundle_count;
}

VkBufferBase &VkBufferBundle::operator[](uint32_t index) const
{
	return *buffers[index];
}

VkBufferBase &VkBufferBundle::GetOne(uint32_t index) const
{
	return *buffers[index];
}

const std::vector<std::shared_ptr<VkBufferBase>> &VkBufferBundle::GetUniformBuffersArray() const

{
	return buffers;
}

