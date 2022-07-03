#pragma once

#include "VkBufferBundle.h"
#include "VkGraphicsComponent.h"
#include "VkStagingBuffer.h"
//class VkStagingBufferFactory
//{
//  public:
//	VkStagingBufferFactory(VkGraphicsComponent &gfx_);
//	~VkStagingBufferFactory() = default;
//
//	VkStagingBufferFactory(const VkStagingBufferFactory &) = delete;
//	VkStagingBufferFactory &operator=(const VkStagingBufferFactory &) = delete;
//	VkStagingBufferFactory(VkStagingBufferFactory &&)                 = delete;
//	VkStagingBufferFactory &operator=(VkStagingBufferFactory &&) = delete;
//
//	class ParameterPack
//	{
//	  public:
//		ParameterPack()
//		{
//			//********************************************************************
//			default_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//
//			default_bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//
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
//	template <VkDeviceSize T,typename CI>
//	[[nodiscard]] std::shared_ptr<VkBufferBase> ProduceBuffer(const CI &para_pack) const;
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
//template <VkDeviceSize T, typename CI>
//std::shared_ptr<VkBufferBase> VkStagingBufferFactory::ProduceBuffer(const CI &para_pack) const
//{
//
//
//
//}
//
