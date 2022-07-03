#pragma once
//#include "VkBufferBundle.h"
//#include "VkBufferFactory.h"
//#include "VkGraphicsComponent.h"
//#include "VkUniformBuffer.h"
//class VkUniformBufferFactory
//{
//  public:
//	VkUniformBufferFactory(VkGraphicsComponent &gfx_);
//	~VkUniformBufferFactory() = default;
//
//	VkUniformBufferFactory(const VkUniformBufferFactory &) = delete;
//	VkUniformBufferFactory &operator=(const VkUniformBufferFactory &) = delete;
//	VkUniformBufferFactory(VkUniformBufferFactory &&)                 = delete;
//	VkUniformBufferFactory &operator=(VkUniformBufferFactory &&) = delete;
//
//	class ParameterPack
//	{
//	  public:
//		ParameterPack()
//		{
//			//********************************************************************
//			default_bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//			default_bufferInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
//			default_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//			//********************************************************************
//			//this memory pro will create memory on cpu side,not gpu side
//			memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
//		}
//
//		VkMemoryPropertyFlags memory_properties{};
//		VkBufferCreateInfo    default_bufferInfo{};
//	};
//
//	template <typename T>
//	[[nodiscard]] std::shared_ptr<VkBufferBase> ProduceBuffer(const ParameterPack &para_pack) const;
//	template <typename T>
//	//VkBufferBundle is copyable without risks of memory leak
//	[[nodiscard]] VkBufferBundle ProduceBufferBundle(uint32_t bundle_size, const ParameterPack &para_pack) const;
//	template <typename T>
//	[[nodiscard]] std::shared_ptr<VkBufferBundle> ProduceBufferBundlePtr(uint32_t bundle_size, const ParameterPack &para_pack) const;
//
//  private:
//	[[nodiscard]] VkBuffer       BuildBuffer(const ParameterPack &para_pack, VkDeviceSize buffer_size) const;
//	[[nodiscard]] VkDeviceMemory BuildMemory(const ParameterPack &para_pack, VkBuffer buffer) const;
//	void                         BindBufferToMemo(VkBuffer buffer, VkDeviceMemory memory) const;
//
//  private:
//	VkGraphicsComponent &  gfx;
//	const VkDeviceManager &device_manager;
//	const VkWindows &      window;
//};
//
//
//
//
//template <typename T>
//std::shared_ptr<VkBufferBase> VkUniformBufferFactory::ProduceBuffer(const ParameterPack &para_pack) const
//{
//	const auto uniform_buffer = BuildBuffer(para_pack, sizeof(T));
//	const auto buffer_memory  = BuildMemory(para_pack, uniform_buffer);
//	BindBufferToMemo(uniform_buffer, buffer_memory);
//	auto result = std::make_shared<VkUniformBuffer>(gfx, uniform_buffer, buffer_memory, sizeof(T));
//	return result;
//}
//
//template <typename T>
//VkBufferBundle VkUniformBufferFactory::ProduceBufferBundle(uint32_t bundle_size, const ParameterPack &para_pack) const
//{
//	std::vector<std::shared_ptr<VkBufferBase>> result_bundle;
//	for (size_t i = 0; i < bundle_size; i++)
//	{
//		result_bundle.push_back(ProduceBuffer<T>(para_pack));
//	}
//	return {std::move(result_bundle), bundle_size};
//}
//
//template <typename T>
//std::shared_ptr<VkBufferBundle> VkUniformBufferFactory::ProduceBufferBundlePtr(uint32_t bundle_size, const ParameterPack &para_pack) const
//{
//	std::vector<std::shared_ptr<VkBufferBase>> result_bundle;
//	for (size_t i = 0; i < bundle_size; i++)
//	{
//		result_bundle.push_back(ProduceBuffer<T>(para_pack));
//	}
//	return std::make_shared<VkBufferBundle>(std::move(result_bundle), bundle_size);
//}
