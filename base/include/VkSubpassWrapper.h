#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDescriptorManager.h"
#include "VkPipelineWrapper.h"
#include <vector>

class VkSubpassWrapper
{
  public:
	VkSubpassWrapper(VkDeviceManager &_device_manager, VkDescriptorManager &_descriptor_manager, uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

	class AttachmentRefMetaInfo
	{
	  public:
		::std::string name;
		uint8_t       slot_to_attch;
		VkImageLayout layout;
	};

	VkSubpassDescription &GetSubpassDescription();
	VkPipelineBindPoint & GetBindPoint();
	[[nodiscard]]uint8_t               GetSubpassNum() const;
	[[nodiscard]]uint8_t               GetRenderpassNum() const;

	void SetPipeline(std::shared_ptr<VkPipelineWrapper> _pipeline);

	std::vector<AttachmentRefMetaInfo> input_attachments_ref;
	std::vector<AttachmentRefMetaInfo> color_attachments_ref;
	std::vector<AttachmentRefMetaInfo> resolve_attachments_ref;
	std::vector<AttachmentRefMetaInfo> depth_attachments_ref;
	std::vector<AttachmentRefMetaInfo> preserve_attachments_ref;

  public:
	[[nodiscard]] VkPipeline GetPipeline() const;
	[[nodiscard]] VkPipelineLayout GetPipelineLayout() const;

  private:
	VkDescriptorManager &descriptor_manager;
	VkDeviceManager &    device_manager;

  private:
	uint8_t renderpass_num;
	uint8_t subpass_num;

	VkPipelineBindPoint  bind_point;
	VkSubpassDescription subpass_description;

	VkPipelineLayout                   pipeline_layout;
	std::shared_ptr<VkPipelineWrapper> pipeline;
};
