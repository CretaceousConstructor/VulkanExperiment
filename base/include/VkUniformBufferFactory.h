#pragma once
#include "VkBufferBundle.h"
#include "VkBufferFactory.h"
#include "VkGraphicsComponent.h"
#include "VkUniformBuffer.h"
class VkUniformBufferFactory : public VkBufferFactory
{
  public:
	VkUniformBufferFactory(VkGraphicsComponent &_gfx);
	~VkUniformBufferFactory() override = default;

	VkUniformBufferFactory(const VkUniformBufferFactory &) = delete;
	VkUniformBufferFactory &operator=(const VkUniformBufferFactory &) = delete;
	VkUniformBufferFactory(VkUniformBufferFactory &&)                 = delete;
	VkUniformBufferFactory &operator=(VkUniformBufferFactory &&) = delete;


	//[[nodiscard]] VkBufferBundle ProduceBufferBundle(VkDeviceSize buffer_size, uint32_t bundle_size, VkMemoryPropertyFlags properties) override;


  protected:
	void BuildBuffer() override;
	void BuildMemory() override;
	void BindBufferToMemo() override;
	void Assemble() override;
	void RestoreToDefaultState() override;

  public:
	void SetSharingMode(VkSharingMode _sharing_mode);

 
  private:
	//the following are states required to be restored
	VkSharingMode default_sharing_mode{VK_SHARING_MODE_EXCLUSIVE};
};
