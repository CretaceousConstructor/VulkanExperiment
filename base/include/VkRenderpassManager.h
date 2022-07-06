#pragma once
#include "VkFactoryBundle.h"
#include "VkManagerBundle.h"
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
	[[nodiscard]] VkPipeline         GetPipeline(const PipelineMetaInfo &meta_info) const;
	[[nodiscard]] VkPipelineLayout   GetPipelineLayout(const PipelineLayoutMetaInfo &meta_info) const;
	[[nodiscard]] VkPipelineBuilder &GetPipelineBuilder();

	[[nodiscard]] const std::vector<VkDescriptorSet> &GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const;

  public:
	VkDescriptorManager &GetDescriptorManager();
	VkPipelineManager &  GetPipelineManager();

	/**
	 * \brief 返回没有状态记忆的工厂类对象
	 * \return stateless factory
	 */
	//const VkSubPassFacotry &       GetSubPassFactory();
	const VkBufferFactory &        GetBufferFactory();
	const VkTextureFactory &       GetTextureFactory();
	const VkSynObjectFactory &     GetSynOjectFactory();
	//const VkDepthImageFactory &    GetDepthImageFactory();
	//const VkSwapchainImageFactory &GetSwapchainImageFactory();

	const VkImageFactory &GetImageFactory();
	const VkModelFactory &         GetModelFactory();

  public:
	VkManagerBundle &GetManagerBundle();
	VkFactoryBundle &GetFactoryBundl();

  private:
	VkGraphicsComponent &gfx;

  private:
	VkManagerBundle managers;
	VkFactoryBundle factories;

  private:
	//VkRenderpassWrapper &                             GetRenderpass(uint32_t pass);

	//std::unordered_map<uint32_t, VkRenderpassWrapper> render_passes;
	//std::unordered_map<uint32_t, std::string>         render_passes_names;
};
