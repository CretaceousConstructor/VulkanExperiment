#pragma once

#include "VkBufferBundle.h"
#include "VkBufferBase.h"

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

	[[nodiscard]]virtual std::shared_ptr<VkBufferBase> ProduceBuffer(VkDeviceSize buffer_size, VkMemoryPropertyFlags memory_properties);
	//VkBufferBundle is copyable without risks of memory leak
	[[nodiscard]]virtual VkBufferBundle                   ProduceBufferBundle(VkDeviceSize buffer_size, uint32_t bundle_size, VkMemoryPropertyFlags properties);
	[[nodiscard]]virtual std::shared_ptr<VkBufferBundle>  ProduceBufferBundlePtr(VkDeviceSize buffer_size, uint32_t bundle_size, VkMemoryPropertyFlags properties);

  protected:
	virtual void BuildBuffer()          = 0;
	virtual void BuildMemory()          = 0;
	virtual void BindBufferToMemo()     = 0;
	virtual void Assemble()             = 0;
	virtual void RestoreToDefaultState() = 0;

  protected:
	std::shared_ptr<VkBufferBase> result;

  private:
  protected:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
	const VkWindows &      window;

  protected:
	//those are temp variables used when contructing buffer.when a call of ProduceBuffer function finishes,those vars are not valid anymore
	VkBuffer              temp_buffer{};
	VkDeviceMemory        temp_buffer_memory{};
	VkDeviceSize          temp_buffer_size{};
	VkMemoryPropertyFlags temp_properties{};

	bool factory_state_modified{false};
};