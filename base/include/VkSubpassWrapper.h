#pragma once

#include "VkGraphicsComponent.h"
#include "VkDescriptorManager.h"
#include "VkPipelineWrapper.h"
#include <vector>

class VkSubpassWrapper
{
  public:
	VkSubpassWrapper( uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point);

	class AttachmentRefMetaInfo
	{
	  public:
		std::string name;
		uint8_t       slot_to_attch;
		VkImageLayout layout;
	};

	VkSubpassDescription& GetSubpassDescription();
	VkPipelineBindPoint & GetBindPoint();
	[[nodiscard]]uint32_t GetSubpassNum() const;
	[[nodiscard]]uint32_t GetRenderpassNum() const;


	std::vector<AttachmentRefMetaInfo> input_attachments_ref;
	std::vector<AttachmentRefMetaInfo> color_attachments_ref;
	std::vector<AttachmentRefMetaInfo> resolve_attachments_ref;
	std::vector<AttachmentRefMetaInfo> depth_attachments_ref;
	std::vector<AttachmentRefMetaInfo> preserve_attachments_ref;


  private:
	//VkGraphicsComponent &gfx;
	//const VkDeviceManager &    device_manager;

  private:
	uint32_t renderpass_num{};
	uint32_t subpass_num{};

	VkPipelineBindPoint  bind_point{};
	VkSubpassDescription subpass_description{};


};
