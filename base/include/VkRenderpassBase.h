#pragma once
#include "VkFactoryBundle.h"
#include "VkManagerBundle.h"
#include "VkRenderpassManager.h"

class VkRenderpassBase
{
  public:
	VkRenderpassBase(VkGraphicsComponent &gfx_,VkRenderpassManager &renderpass_manager_);
	virtual ~VkRenderpassBase() = default;

	VkRenderpassBase()                         = delete;
	VkRenderpassBase(const VkRenderpassBase &) = delete;
	VkRenderpassBase &operator=(const VkRenderpassBase &) = delete;
	VkRenderpassBase(VkRenderpassBase &&)                 = delete;
	VkRenderpassBase &operator=(VkRenderpassBase &&) = delete;

	void         Init();
	virtual void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)   = 0;
	virtual void ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers) = 0;
	virtual void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)     = 0;

	virtual void UpdateResources(size_t currentImage) = 0;

  protected:
	virtual void ResourceInit()              = 0;
	virtual void CreateDescriptorSetLayout() = 0;
	virtual void CreateDescriptorSetsThenUpdate()      = 0;

	virtual void CreateAttachments() = 0;

	virtual void CreateGraphicsPipelineLayout() = 0;
	virtual void CompileShaders()               = 0;

  protected:
	VkGraphicsComponent &gfx;
	VkRenderpassManager &renderpass_manager;
};
