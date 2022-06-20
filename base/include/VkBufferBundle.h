#pragma once
#include "VkBufferBase.h"
//THIS CLASS IS COPYABLE,NO MEMORY LEAK WILL HAPPEND
class VkBufferBundle
{
  public:
	VkBufferBundle() = delete;
	VkBufferBundle(std::vector<std::shared_ptr<VkBufferBase>> uniform_buffers, uint32_t bundle_size);
	~VkBufferBundle() = default;

	VkBufferBundle(const VkBufferBundle &) = default;
	VkBufferBundle &operator=(const VkBufferBundle &) = default;

	VkBufferBundle(VkBufferBundle &&)                 = default;
	VkBufferBundle &operator=(VkBufferBundle &&) = default;


	[[nodiscard]] uint32_t GetBundleCount() const;
	const VkBufferBase &operator[](uint32_t index) const;
	[[nodiscard]] const VkBufferBase &GetOne(uint32_t index) const;
	[[nodiscard]] const std::vector<std::shared_ptr<VkBufferBase>> &GetUniformBuffersArray() const;


private:
	uint32_t bundle_count{};
	const std::vector<std::shared_ptr<VkBufferBase>> buffers;



};