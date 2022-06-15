#pragma once

#include "VkUniformBuffer.h"


class VkUniformBufferBundle
{
  public:
	VkUniformBufferBundle() = delete;
	VkUniformBufferBundle(std::vector<std::unique_ptr<VkUniformBuffer>> &&_uniform_buffers, uint8_t bundle_size);

	[[nodiscard]] uint8_t GetBundleCount() const;


	VkUniformBuffer &operator[](uint8_t index) const;

	[[nodiscard]] VkUniformBuffer &GetOne(uint8_t index) const;



	[[nodiscard]] const std::vector<std::unique_ptr<VkUniformBuffer>> &GetUniformBuffersArray() const;



private:
	uint8_t bundle_count;
	std::vector<std::unique_ptr<VkUniformBuffer>> uniform_buffers;




};
