#pragma once
#include "VkDescriptorManagerV0.h"
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
	template <typename First, typename... Rest>
	static std::shared_ptr<First> ProduceRenderpass(Rest &&...rest);

  public:
	void AddRenderPass(
	    const std::string &name, uint32_t slot, const std::vector<VkAttachmentInfo> &attachments,
	    const std::vector<VkSubpassDependency> &dependencies, const std::vector<std::shared_ptr<VkSubpassWrapper>> &subpasses);

	void AddPipeline(const VkPipelinePP &para_pack, const PipelineMetaInfo &meta_info);

	void AddPipelineLayout(const PipelineLayoutMetaInfo &pipe_layout_info);

  public:
	[[nodiscard]] VkPipeline         GetPipeline(const PipelineMetaInfo &meta_info) const;
	[[nodiscard]] VkPipelineLayout   GetPipelineLayout(const PipelineLayoutMetaInfo &meta_info) const;
	[[nodiscard]] VkPipelineBuilder &GetPipelineBuilder();

	[[nodiscard]] const std::vector<VkDescriptorSet> &GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const;

  public:
  public:
	VkDescriptorManager &  GetDescriptorManager();
	VkDescriptorManagerV0 &GetDescriptorManagerV0();
	VkPipelineManager &    GetPipelineManager();

	/**
	 * \brief 返回没有状态记忆的工厂类对象
	 * \return stateless factory
	 */
	//const VkSubPassFacotry &       GetSubPassFactory();

	const VkBufferFactory &GetBufferFactory();

	const VkTextureFactory &GetTextureFactory();

	VkSynObjectFactory &GetSynOjectFactory();

	const VkImageFactory &GetImageFactory();
	const VkModelFactory &GetModelFactory();

	const VkDescriptorSetFactory &GetDescriptorSetFactory();
	const VkShaderFactory &       GetShaderFactory();

  public:
	VkManagerBundle &GetManagerBundle();
	VkFactoryBundle &GetFactoryBundle();

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

template <typename First, typename... Rest>
std::shared_ptr<First> VkRenderpassManager::ProduceRenderpass(Rest &&...rest)
{
	std::shared_ptr<First> result = std::make_shared<First>(std::forward<Rest...>(rest...));
	result->Init();
	return result;
}
