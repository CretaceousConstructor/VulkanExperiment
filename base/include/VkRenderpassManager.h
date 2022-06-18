#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkWindows.h"
#include "ShaderWrapper.h"
#include "VkDeviceManager.h"
#include "VkSwapChainManager.h"
#include "VkDescriptorManager.h"
#include "VkRenderpassWrapper.h"
#include "VkPipelineBuilder.h"
#include "VkAttachmentInfo.h"
#include "VkSubpassFactory.h"
#include "VkSynObjectFactory.h"
#include "VkTextureFactory.h"
#include "VkUniformBufferFactory.h"

#include <unordered_map>
#include <map>


class VkRenderpassManager
{

   public:
	VkRenderpassManager(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager,VkWindows& _window,VkCommandManager &_command_manager);
	 ~VkRenderpassManager() = default;

	VkRenderpassManager(const VkRenderpassManager &) = delete;
	VkRenderpassManager &operator=(const VkRenderpassManager &) = delete;
	VkRenderpassManager(VkRenderpassManager&&) = delete;
	VkRenderpassManager &operator=(VkRenderpassManager&&) = delete;



	void AddRenderPass(const std::string& name, uint8_t slot, std::vector<VkAttachmentInfo> attachments, std::vector<VkSubpassDependency> dependencies, std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses);

	void AddPipeline(const std::string name,PipelineMetaInfo meta_info,const std::vector<ShaderWrapper::ShaderInfo> shader_infos);

	[[nodiscard]] VkPipeline       GetPipeline(PipelineMetaInfo meta_info) const;
	[[nodiscard]] VkPipelineLayout GetPipelineLayout(PipelineMetaInfo meta_info) const;

	VkRenderpassWrapper &GetRenderpass(uint8_t pass);
	const std::vector<VkDescriptorSet>& GetDescriptorSetBundle(DescriptorMetaInfo meta_info);
  public:
	VkDescriptorManager &GetDescriptorManager();
	VkSubPassFacotry    &GetSubPassFactory();

	VkUniformBufferFactory    &GetUniformBufferFactory();
	VkTextureFactory          &GetTextureFactory();
	VkSynObjectFactory        &GetSynOjectFactory();

public:
	VkPipelineBuilder   &GetPipelineBuilder();




  private:

	VkDeviceManager &    device_manager;
	VkSwapChainManager&  swapchain_manager;
	VkWindows &window;
	VkCommandManager&     command_manager;


  private:
	//第一个uint8表示第几个renderpass
	std::unordered_map<uint8_t, VkRenderpassWrapper> render_passes;
	std::unordered_map<uint8_t, std::string>         render_passes_names;

  private:
	//DESCRIPTOR MAN(layouts and pools and sets)
	VkDescriptorManager  descriptor_manager;
	//BUILDER FOR BUILDING PIPELINE以后可以做成指针，从而做到运行时切换不同的构建者
	VkPipelineBuilder pipeline_builder;

	VkSubPassFacotry subpass_factory;

	VkUniformBufferFactory ubuffer_factory;
	VkTextureFactory       texture_factory;
	VkSynObjectFactory     syn_obj_factory;


};

