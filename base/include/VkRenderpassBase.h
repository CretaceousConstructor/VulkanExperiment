#pragma once
#include "VkFactoryBundle.h"
#include "VkManagerBundle.h"
#include "VkRenderpassManager.h"

class VkRenderpassBase
{
  public:
	VkRenderpassBase(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_);
	virtual ~VkRenderpassBase() = default;

	VkRenderpassBase()                         = delete;
	VkRenderpassBase(const VkRenderpassBase &) = delete;
	VkRenderpassBase &operator=(const VkRenderpassBase &) = delete;
	VkRenderpassBase(VkRenderpassBase &&)                 = delete;
	VkRenderpassBase &operator=(VkRenderpassBase &&) = delete;

  public:
	void Init();
	void Execute(const std::vector<VkCommandBuffer> &command_buffers);

  protected:
	virtual void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)   = 0;
	virtual void UpdateDescriptorSets()                                                 = 0;
	virtual void ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers) = 0;
	virtual void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)     = 0;

  public:
	virtual void UpdateResources(size_t current_image) = 0;

  protected:
	virtual void                          LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) = 0;
	virtual std::vector<VkAttachmentInfo> SelectAttachments(std::optional<size_t> current_image);
	virtual void                          LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) = 0;

  protected:
	virtual void ResourceInit() = 0;

	virtual void CreateLocalCommandBuffers() = 0;
	virtual void CreateDescriptorSetPools()  = 0;
	virtual void CreateDescriptorSetLayout() = 0;
	virtual void CreateDescriptorSets()      = 0;

	virtual void CreateAttachments()            = 0;
	virtual void CreateGraphicsPipelineLayout() = 0;
	virtual void CreateShaders()                = 0;
	virtual void CreateGraphicsPipeline()       = 0;

  protected:
	VkGraphicsComponent &gfx;
	VkRenderpassManager &renderpass_manager;
};
