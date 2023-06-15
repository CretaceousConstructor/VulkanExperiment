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
	void ExecuteStatically(const std::vector<VkCommandBuffer> &command_buffers);

public:
	//void InitRG();

	virtual void ResourceInitRG(){} ;

	virtual void CreateLocalCommandBuffersRG(){} ;
	virtual void CreateDescriptorSetPoolsRG(){} ;
	virtual void CreateDescriptorSetLayoutRG(){} ;
	virtual void CreateDescriptorSetsRG(){}      ;

	virtual void CreateAttachmentsRG(std::vector<VkAttachmentInfo> attachment_infos){};
	virtual void CreateUniformBufferDescriptorsRG(std::vector<VkUniBufUsageInfo> attachment_infos){};


	virtual void CreateGraphicsPipelineLayoutRG(){} ;
	virtual void CreateShadersRG(){}                ;
	virtual void CreateGraphicsPipelineRG(){}       ;

public:

	virtual void BeginRenderpassRG(const VkCommandBuffer command_buffers) {}
	virtual void UpdateDescriptorSetsRG() {}
	virtual void RecordRenderpassCommandRG(const VkCommandBuffer command_buffers) {}
	virtual void EndRenderpassRG(const VkCommandBuffer command_buffers) {}


  public:
	enum class Type
	{
		Graphics,
		Compute,
		Transfor,
		Unknown
	};

  public:
	virtual void UpdateResources(size_t current_image) = 0;

  protected:
	virtual void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)             = 0;
	virtual void UpdateDescriptorSets()                                                           = 0;
	virtual void RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers) = 0;
	virtual void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)               = 0;


  protected:
	virtual void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) = 0;
	virtual void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)   = 0;

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





//template <typename First, typename... Rest>
//std::shared_ptr<First> VkRenderpassBase::ProduceRenderpass(Rest &&...rest)
//{
//	std::shared_ptr<First> result = std::make_shared<First>(rest...);
//	result->Init();
//	return result;
//}
