#pragma once
#include "VkFactoryBundle.h"
#include "VkManagerBundle.h"
#include "VkRenderpassManager.h"

class VkRenderpassBase
{
  public:
	VkRenderpassBase(VkGraphicsComponent &gfx_,
	                 VkRenderpassManager &renderpass_manager_);
	virtual ~VkRenderpassBase()                = default;


	VkRenderpassBase()                         = delete;
	VkRenderpassBase(const VkRenderpassBase &) = delete;
	VkRenderpassBase &operator=(const VkRenderpassBase &) = delete;
	VkRenderpassBase(VkRenderpassBase &&)                 = delete;
	VkRenderpassBase &operator=(VkRenderpassBase &&) = delete;

	void Init();

	virtual void CreateDescriptorSetLayout() = 0;
	virtual void CreateDescriptorSets()      = 0;

	virtual void CreateRenderPass() = 0;

	virtual void CreateGraphicsPipelineLayout() = 0;
	virtual void CompileShaders()               = 0;
	virtual void CreateGraphicsPipeline()       = 0;

	virtual VkRenderPassBeginInfo StartRenderpass(uint32_t index) const = 0;


  protected:
	VkGraphicsComponent &gfx;
	VkRenderpassManager &renderpass_manager;


};
