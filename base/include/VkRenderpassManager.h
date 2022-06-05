#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "ShaderManager.h"
#include "VkDeviceManager.h"
#include "VkWindows.h"
#include "VkSwapChainManager.h"
#include "VkDescriptorManager.h"
#include "VkRenderpassWrapper.h"
#include "VkPipelineBuilder.h"

#include <unordered_map>


class VkRenderpassManager
{

   public:
	VkRenderpassManager(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager);
	~VkRenderpassManager();
	VkDescriptorManager &GetDescriptorManager();
	

	void AddRenderPass(const std::string name, uint8_t slot, std::vector<VkAttachmentDescription> attachments, std::vector<VkSubpassDependency> dependencies, std::vector<VkSubpassWrapper> subpasses);
	void AddPipeline(const std::string name,PipelineMetaInfo meta_info);

	VkPipelineBuilder& GetPipelineBuilder();


  private:
	VkDeviceManager &    device_manager;
	VkSwapChainManager&  swapchain_manager;



  private:
	//第一个uint8表示第几个renderpass
	std::map<uint8_t, VkRenderpassWrapper> render_passes;
	std::map<uint8_t, std::string> render_passes_names;

  private:
	//DESCRIPTOR MAN(layout and pool and sets)
	VkDescriptorManager  descriptor_manager;
	//Builder for building pipeline
	VkPipelineBuilder pipeline_builder;



};

