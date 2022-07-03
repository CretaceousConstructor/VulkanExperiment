#include "VkUniformBufferFactory.h"

//VkUniformBufferFactory::VkUniformBufferFactory(VkGraphicsComponent &gfx_) :
//    gfx(gfx_),
//    device_manager(gfx_.DeviceMan()),
//    window(gfx_.Window())
//{
//}
//
////std::shared_ptr<VkBufferBase> VkUniformBufferFactory::ProduceBuffer(VkDeviceSize buffer_size, const ParameterPack &para_pack) const
////{
////	const auto uniform_buffer = BuildBuffer(para_pack, buffer_size);
////	const auto buffer_memory  = BuildMemory(para_pack, uniform_buffer);
////	BindBufferToMemo(uniform_buffer, buffer_memory);
////
////	auto result = std::make_shared<VkUniformBuffer>(gfx, uniform_buffer, buffer_memory, buffer_size);
////	return result;
////}
//
////VkBufferBundle VkUniformBufferFactory::ProduceBufferBundle(VkDeviceSize buffer_size, uint32_t bundle_size, const ParameterPack &para_pack) const
////{
////	std::vector<std::shared_ptr<VkBufferBase>> result_bundle;
////	for (size_t i = 0; i < bundle_size; i++)
////	{
////		result_bundle.push_back(ProduceBuffer(buffer_size, para_pack));
////	}
////	return {std::move(result_bundle), bundle_size};
////}
//
////std::shared_ptr<VkBufferBundle> VkUniformBufferFactory::ProduceBufferBundlePtr(VkDeviceSize buffer_size, uint32_t bundle_size, const ParameterPack &para_pack) const
////{
////	std::vector<std::shared_ptr<VkBufferBase>> result_bundle;
////	for (size_t i = 0; i < bundle_size; i++)
////	{
////		result_bundle.push_back(ProduceBuffer(buffer_size, para_pack));
////	}
////	return std::make_shared<VkBufferBundle>(std::move(result_bundle), bundle_size);
////}
//
//VkBuffer VkUniformBufferFactory::BuildBuffer(const ParameterPack &para_pack, VkDeviceSize buffer_size) const
//{
//	/*typedef struct VkBufferCreateInfo {
//		VkStructureType        sType;
//		const void* pNext;
//		VkBufferCreateFlags    flags;
//		VkDeviceSize           size;
//		VkBufferUsageFlags     usage;
//		VkSharingMode          sharingMode;
//		uint32_t               queueFamilyIndexCount;
//		const uint32_t* pQueueFamilyIndices;
//	} VkBufferCreateInfo;*/
//
//	VkBuffer result{};
//
//	ParameterPack para_pack_local(para_pack);
//	para_pack_local.default_bufferInfo.size = buffer_size;
//
//	const VkDeviceManager::QueueFamilyIndices indices            = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());
//	const std::set<uint32_t>                  queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};
//
//	std::vector<uint32_t> uniqueQueueFamilyIndices;
//
//	for (auto index : queueFamilyIndices)
//	{
//		uniqueQueueFamilyIndices.push_back(index);
//	}
//
//	if (para_pack_local.default_bufferInfo.sharingMode == VK_SHARING_MODE_CONCURRENT)
//	{
//		para_pack_local.default_bufferInfo.pQueueFamilyIndices   = uniqueQueueFamilyIndices.data();
//		para_pack_local.default_bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
//	}
//
//	if (vkCreateBuffer(device_manager.GetLogicalDevice(), &para_pack_local.default_bufferInfo, nullptr, &result) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create buffer!");
//	}
//	return result;
//}
//
//VkDeviceMemory VkUniformBufferFactory::BuildMemory(const ParameterPack &para_pack, VkBuffer buffer) const
//{
//	VkDeviceMemory       result{};
//	VkMemoryRequirements memRequirements;
//	vkGetBufferMemoryRequirements(device_manager.GetLogicalDevice(), buffer, &memRequirements);
//
//	VkMemoryAllocateInfo allocInfo{};
//	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize = memRequirements.size;
//
//	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, para_pack.memory_properties, device_manager.GetPhysicalDevice());
//
//	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &result) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate buffer memory!");
//	}
//	return result;
//}
//
//void VkUniformBufferFactory::BindBufferToMemo(VkBuffer buffer, VkDeviceMemory memory) const
//{
//	vkBindBufferMemory(device_manager.GetLogicalDevice(), buffer, memory, 0);
//}
