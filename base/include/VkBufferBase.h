#pragma once
#include "VkGraphicsComponent.h"

class VkBufferBase
{
	friend class VkBufferFactory;

  public:
	VkBufferBase(VkGraphicsComponent &_gfx, const VkBuffer _buffer, const VkDeviceMemory _buffer_memory, VkDeviceSize _size);

  public:
	~VkBufferBase();

	VkBufferBase()                     = delete;
	VkBufferBase(const VkBufferBase &) = delete;
	VkBufferBase &operator=(const VkBufferBase &) = delete;
	VkBufferBase(VkBufferBase &&)                 = delete;
	VkBufferBase &operator=(VkBufferBase &&) = delete;

	[[nodiscard]] const VkBuffer &GetBuffer() const;
	VkWriteDescriptorSet          GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement = 0, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	void Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset_in_whole_mem = 0) const;

	void Unmap();

	void CopyTo(void const *outside_data_to_be_mapped, size_t outside_data_size, VkDeviceSize mapped_region_starting_offset = 0) const;

	void Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset_in_whole_mem = 0) const;

	[[nodiscard]] void *GetPtrToMappedRegion() const;

  private:
	//void MapMemory(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);这个函数要弄成private，否则Map两次中间不unmap会出问题
	void MapMemory(VkDeviceSize size, VkDeviceSize offset);



  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  private:
	VkBuffer               buffer{};
	VkDeviceMemory         buffer_memory{};
	VkDeviceSize           size_of_buffer{};
	VkDescriptorBufferInfo buffer_des_info_write{};
	void *                 mapped{nullptr};
};
