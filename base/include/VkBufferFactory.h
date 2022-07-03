#pragma once

#include "VkBufferBase.h"
#include "VkBufferBundle.h"
#include "VkBufferCI.h"

class VkBufferFactory
{
  public:
	VkBufferFactory(VkGraphicsComponent &_gfx);
	virtual ~VkBufferFactory() = default;

	VkBufferFactory()                        = delete;
	VkBufferFactory(const VkBufferFactory &) = delete;
	VkBufferFactory &operator=(const VkBufferFactory &) = delete;
	VkBufferFactory(VkBufferFactory &&)                 = delete;
	VkBufferFactory &operator=(VkBufferFactory &&) = delete;

	template <typename CI>
	[[nodiscard]] std::shared_ptr<VkBufferBase> ProduceBuffer(VkDeviceSize N, const CI &para_pack) const;

	//VkBufferBundle is copyable without risks of memory leak
	template <typename CI>
	[[nodiscard]] VkBufferBundle ProduceBufferBundle(VkDeviceSize N, uint32_t bundle_size, const CI &para_pack) const;

	template <typename CI>
	[[nodiscard]] std::shared_ptr<VkBufferBundle> ProduceBufferBundlePtr(VkDeviceSize N, uint32_t bundle_size, const CI &para_pack) const;

  private:
	template <typename CI>
	[[nodiscard]] VkBuffer BuildBuffer(const CI &para_pack, VkDeviceSize buffer_size) const;
	template <typename CI>
	[[nodiscard]] VkDeviceMemory BuildMemory(const CI &para_pack, VkBuffer buffer) const;
	void                         BindBufferToMemo(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize offset = 0) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};

template <typename CI>
std::shared_ptr<VkBufferBase> VkBufferFactory::ProduceBuffer(VkDeviceSize N, const CI &para_pack) const
{
	//static_assert(std::is_same_v<CI, VkBufferCI::Buffer<>>);

	const auto uniform_buffer = BuildBuffer<CI>(para_pack, N);
	const auto buffer_memory  = BuildMemory<CI>(para_pack, uniform_buffer);
	BindBufferToMemo(uniform_buffer, buffer_memory);
	auto result = std::make_shared<VkBufferBase>(gfx, uniform_buffer, buffer_memory, N);

	return result;
}

template <typename CI>
VkBufferBundle VkBufferFactory::ProduceBufferBundle(VkDeviceSize N, uint32_t bundle_size, const CI &para_pack) const
{
	std::vector<std::shared_ptr<VkBufferBase>> result_bundle;
	for (size_t i = 0; i < bundle_size; i++)
	{
		result_bundle.push_back(ProduceBuffer<CI>(N, para_pack));
	}
	return {std::move(result_bundle), bundle_size};
}

template <typename CI>
std::shared_ptr<VkBufferBundle> VkBufferFactory::ProduceBufferBundlePtr(VkDeviceSize N, uint32_t bundle_size, const CI &para_pack) const
{
	std::vector<std::shared_ptr<VkBufferBase>> result_bundle;
	for (size_t i = 0; i < bundle_size; i++)
	{
		result_bundle.push_back(ProduceBuffer<CI>(N, para_pack));
	}
	return std::make_shared<VkBufferBundle>(std::move(result_bundle), bundle_size);
}

template <typename CI>
VkBuffer VkBufferFactory::BuildBuffer(const CI &para_pack, VkDeviceSize buffer_size) const
{
	/*typedef struct VkBufferCreateInfo {
		VkStructureType        sType;
		const void* pNext;
		VkBufferCreateFlags    flags;
		VkDeviceSize           size;
		VkBufferUsageFlags     usage;
		VkSharingMode          sharingMode;
		uint32_t               queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
	} VkBufferCreateInfo;*/

	VkBuffer           result{};
	VkBufferCreateInfo buffer_CI{};
	buffer_CI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_CI.size  = buffer_size;
	buffer_CI.usage = para_pack.usage;
	assert(para_pack.sharing_mode != VK_SHARING_MODE_CONCURRENT);

	if (vkCreateBuffer(device_manager.GetLogicalDevice(), &buffer_CI, nullptr, &result) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}
	return result;
}

template <typename CI>
VkDeviceMemory VkBufferFactory::BuildMemory(const CI &para_pack, VkBuffer buffer) const
{
	VkDeviceMemory       result{};
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device_manager.GetLogicalDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, para_pack.memory_properties, device_manager.GetPhysicalDevice());

	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &result) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	return result;
}
