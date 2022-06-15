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
#include "VkAttachmentInfo.h"
#include <unordered_map>



class VkRenderpassManager
{

   public:
	VkRenderpassManager(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager);
	~VkRenderpassManager();

	VkRenderpassManager(const VkRenderpassManager &) = delete;
	VkRenderpassManager &operator=(const VkRenderpassManager &) = delete;
	VkRenderpassManager(VkRenderpassManager&&) = delete;
	VkRenderpassManager &operator=(VkRenderpassManager&&) = delete;



	void AddRenderPass(const std::string& name, uint8_t slot, std::vector<VkAttachmentInfo> attachments, std::vector<VkSubpassDependency> dependencies, std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses);

	void AddPipeline(const std::string name,PipelineMetaInfo meta_info);



	VkRenderpassWrapper &GetRenderpass(uint8_t pass);

  public:
	VkDescriptorManager &GetDescriptorManager();
	VkSubPassFacotry    &GetSubPassFactory();
public:
	VkPipelineBuilder   &GetPipelineBuilder();




  private:
	//第一个uint8表示第几个renderpass
	std::map<uint8_t, VkRenderpassWrapper> render_passes;
	std::map<uint8_t, std::string>         render_passes_names;

  private:
	//DESCRIPTOR MAN(layouts and pools and sets)
	VkDescriptorManager  descriptor_manager;
	//BUILDER FOR BUILDING PIPELINE以后可以做成指针，从而做到运行时切换不同的构建者
	VkPipelineBuilder pipeline_builder;

	VkSubPassFacotry subpass_factory;

  private:
	VkDeviceManager &    device_manager;
	VkSwapChainManager&  swapchain_manager;

};

