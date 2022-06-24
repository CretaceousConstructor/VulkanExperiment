
#pragma once
#include "VkGraphicsComponent.h"
#include "VkImageBundle.h"
class VkImageFactory
{
  public:
	VkImageFactory(VkGraphicsComponent &_gfx);
	virtual ~VkImageFactory() = default;

	VkImageFactory() = delete;
	VkImageFactory(const VkImageFactory &) = delete;
	VkImageFactory &operator=(const VkImageFactory &) = delete;

	VkImageFactory(VkImageFactory &&) = delete;
	VkImageFactory &operator=(VkImageFactory &&) = delete;




	virtual std::shared_ptr<VkImageBase> ProduceImage();


	//VkBufferBundle is copyable without risks of memory leak
	virtual VkImageBundle ProduceImageBundle(uint32_t bundle_size);
	virtual std::shared_ptr<VkImageBundle> ProduceImageBundlePtr(uint32_t bundle_size);



  protected:

	virtual void BuildImage()            = 0;
	virtual void CreateAndBindMemory()            = 0;
	virtual void BuildImageView()        = 0;
	virtual void Assemble()              = 0;
	virtual void TransitionImageLayout() = 0;
	virtual void RestoreToDefaultState() = 0;

  protected:
	std::shared_ptr<VkImageBase> result;

  protected:
	VkGraphicsComponent &     gfx;

	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkCommandManager &  command_manager;
	const VkWindows &         window;


	//those are temp variables used when contructing buffer.when the call of ProduceBuffer function finishes,those vars are not valid anymore
  protected:

	bool factory_state_modified{false};

};
