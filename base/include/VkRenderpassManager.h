#pragma once
#include "VkGraphicsComponent.h"
#include "VkAttachmentInfo.h"
#include "VkDescriptorManager.h"
#include "VkPipelineBuilder.h"
#include "VkPipelineManager.h"
#include "VkRenderpassWrapper.h"
#include "VkShaderManager.h"
#include "VkSubpassFactory.h"
#include "VkSynObjectFactory.h"
#include "VkTextureFactory.h"
#include "VkUniformBufferFactory.h"
#include "VkTexImageFactory.h"
#include "VkDepthImageFactory.h"
#include <unordered_map>

class VkRenderpassManager
{
  public:
	VkRenderpassManager(VkGraphicsComponent &_gfx);
	~VkRenderpassManager() = default;


	VkRenderpassManager()                            = delete;
	VkRenderpassManager(const VkRenderpassManager &) = delete;
	VkRenderpassManager &operator=(const VkRenderpassManager &) = delete;
	VkRenderpassManager(VkRenderpassManager &&)                 = delete;
	VkRenderpassManager &operator=(VkRenderpassManager &&) = delete;

  public:
	void AddRenderPass(
	    const std::string &name, uint32_t slot, const std::vector<VkAttachmentInfo> &attachments,
	    const std::vector<VkSubpassDependency> &dependencies, const std::vector<std::shared_ptr<VkSubpassWrapper>> &subpasses);

	void AddPipeline(const VkPipelineParameterPack &para_pack, const PipelineMetaInfo &meta_info);

	void AddPipelineLayout(const PipelineLayoutMetaInfo &pipe_layout_info);




  public:
	[[nodiscard]] VkPipeline            GetPipeline(const PipelineMetaInfo& meta_info) const;
	[[nodiscard]] VkPipelineLayout      GetPipelineLayout(const PipelineLayoutMetaInfo& meta_info) const;



	VkRenderpassWrapper &               GetRenderpass(uint8_t pass);
	[[nodiscard]] const std::vector<VkDescriptorSet> &GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const;

  public:
	VkDescriptorManager &GetDescriptorManager();
	VkPipelineManager &GetPipelineManager();


	VkSubPassFacotry &   GetSubPassFactory();
	VkUniformBufferFactory &GetUniformBufferFactory();
	VkTextureFactory &      GetTextureFactory();
	VkSynObjectFactory &    GetSynOjectFactory();

  public:

  private:
	VkGraphicsComponent &     gfx;
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkWindows &         window;
	const VkCommandManager &  command_manager;

  private:
	//DESCRIPTOR MANs(layouts and pools and sets)
	VkDescriptorManager descriptor_manager;
	VkPipelineManager   pipeline_manager;
	//FACTORIES
	VkSubPassFacotry       subpass_factory;
	VkUniformBufferFactory ubuffer_factory;
	VkTexImageFactory      tex_image_factory;
	VkDepthImageFactory    depth_image_factory;
	VkSynObjectFactory     syn_obj_factory;
	VkTextureFactory       texture_factory;

	
  private:
	std::unordered_map<uint32_t, VkRenderpassWrapper> render_passes;
	std::unordered_map<uint32_t, std::string>         render_passes_names;







};
