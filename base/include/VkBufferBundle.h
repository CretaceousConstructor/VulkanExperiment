#pragma once
#include "VkBufferBase.h"
//THIS CLASS IS COPYABLE,NO MEMORY LEAK WILL HAPPEND
class VkBufferBundle
{
  public:
	VkBufferBundle() = default;
	VkBufferBundle(std::vector<std::shared_ptr<VkBufferBase>> buffers, uint32_t bundle_size);
	~VkBufferBundle() = default;

	VkBufferBundle(const VkBufferBundle &) = default;
	VkBufferBundle &operator=(const VkBufferBundle &) = default;

	VkBufferBundle(VkBufferBundle &&)                 = default;
	VkBufferBundle &operator=(VkBufferBundle &&) = default;


	[[nodiscard]] uint32_t GetBundleCount() const;
	VkBufferBase &   operator[](size_t index) const;
	[[nodiscard]] VkBufferBase & GetOne(size_t index) const;

	[[nodiscard]] const std::vector<std::shared_ptr<VkBufferBase>> &GetBufferArray() const;


private:
	uint32_t bundle_count{};
	std::vector<std::shared_ptr<VkBufferBase>> buffers;



};