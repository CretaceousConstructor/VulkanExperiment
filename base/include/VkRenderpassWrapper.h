#pragma once


#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkSubpassWrapper.h"
#include <vector>

class VkRenderpassWrapper
{
  public:
	VkRenderpassWrapper(const std::string &_renderpass_name, std::vector<VkAttachmentDescription> _attachments, std::vector<VkSubpassDependency> _dependencies, std::vector<VkSubpassWrapper> _subpasses, VkDeviceManager &device_manager);


	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkSubpassDependency>     dependencies;

	std::vector<VkSubpassWrapper> subpasses;



	VkRenderPass render_pass;
	std::string  render_pass_name;




};
