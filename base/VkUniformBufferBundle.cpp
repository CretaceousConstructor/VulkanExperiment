#include "VkUniformBufferBundle.h"

VkUniformBufferBundle::VkUniformBufferBundle(std::vector<std::unique_ptr<VkUniformBuffer>> &&_uniform_buffers, uint8_t bundle_size)

    :
    bundle_count(bundle_size),
    uniform_buffers(std::move(_uniform_buffers))
{}

uint8_t VkUniformBufferBundle::GetBundleCount() const
{
	return bundle_count;
}

VkUniformBuffer &VkUniformBufferBundle::operator[](uint8_t index) const
{
	return *uniform_buffers[index];
}

VkUniformBuffer &VkUniformBufferBundle::GetOne(uint8_t index) const
{
	return *uniform_buffers[index];
}

const std::vector<std::unique_ptr<VkUniformBuffer>> &VkUniformBufferBundle::GetUniformBuffersArray() const

{
	return uniform_buffers;
}
