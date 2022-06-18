#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkWindows.h"


#include <vector>



//TODO:把cpu端的数据整合到这个类里面
class VkUniformBuffer
{



public:

	VkUniformBuffer(VkDeviceManager &_device_manager,VkWindows &  window,VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharing_mode );

	  ~VkUniformBuffer();

	VkUniformBuffer(const VkUniformBuffer &) = delete;
	VkUniformBuffer &operator=(const VkUniformBuffer &) = delete;
	VkUniformBuffer &operator=(VkUniformBuffer&&) = delete;
	VkUniformBuffer(VkUniformBuffer &&) = delete;




	VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement) ;





	void MapMemory(VkDeviceSize mapped_region_starting_offset, VkDeviceSize mapped_region_size, void const *outside_data_to_be_mapped, size_t outside_data_size, VkMemoryMapFlags flgs = 0) const;



private:
	VkDeviceManager &device_manager;
	VkWindows &      window;
private:
	VkDescriptorBufferInfo buffer_info_write{};
  private:
	VkBuffer buffer{nullptr};
	VkDeviceMemory memory{nullptr};
	VkDeviceSize           size_of_buffer;


};













