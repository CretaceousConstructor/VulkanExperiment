#pragma once


#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkSubpassWrapper.h"
#include "VkAttachmentInfo.h"
#include <vector>

class VkRenderpassWrapper
{
  public:
	VkRenderpassWrapper(const std::string &_renderpass_name, std::vector<VkAttachmentInfo> _attachment_infos, std::vector<VkSubpassDependency> _dependencies, 	std::vector<std::shared_ptr<VkSubpassWrapper>>  _subpasses, VkDeviceManager &device_manager);


	std::vector<VkAttachmentInfo> attachment_infos;
	std::vector<VkSubpassDependency>     dependencies;
	std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses;

	VkRenderPass render_pass;
	std::string  render_pass_name;

	std::vector<VkFramebuffer> frame_buffers;


};
