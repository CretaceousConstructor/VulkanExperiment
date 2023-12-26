#pragma once
#include "VkFactoryBundle.h"
#include "VkManagerBundle.h"
#include "VkRenderpassManager.h"

namespace RenderGraphV0
{
	class PassNode;
}


class VkRenderpassBaseRG
{
  public:
	void Init();
	void Execute(const VkCommandBuffer command_buffer);

  public:
	void SetRenderGraphPassHandle(size_t handle);
	void SetRenderGraphPassPointer(RenderGraphV0::PassNode *curr_pass_in_rg_);

  public:
	enum Type 
	{
		None = 0x00,
		Graphics = 0x01,
		Compute  = 0x02,
		Transfor = 0x04,
		Present = 0x04,
	};



  public:
	virtual void UpdateResources(size_t current_image) ;

  public:
	VkRenderpassBaseRG(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_);
	virtual ~VkRenderpassBaseRG() = default;

	VkRenderpassBaseRG()                           = delete;
	VkRenderpassBaseRG(const VkRenderpassBaseRG &) = delete;
	VkRenderpassBaseRG &operator=(const VkRenderpassBaseRG &) = delete;
	VkRenderpassBaseRG(VkRenderpassBaseRG &&)                 = delete;
	VkRenderpassBaseRG &operator=(VkRenderpassBaseRG &&) = delete;

  protected:
	virtual void BeginRenderpass(const VkCommandBuffer cmd_buf)           = 0;
	virtual void UpdateDescriptorSets()                                   = 0;
	virtual void RecordRenderpassCommandRG(const VkCommandBuffer cmd_buf) = 0;
	virtual void EndRenderpass(const VkCommandBuffer cmd_buf)             = 0;

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
	virtual void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer);
	virtual void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer);



  protected:
	VkGraphicsComponent &gfx;
	VkRenderpassManager &renderpass_manager;

  protected:
	size_t pass_handle_inRG{};
	RenderGraphV0::PassNode *curr_pass_in_rg{};

};

