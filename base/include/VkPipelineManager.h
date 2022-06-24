#pragma once
#include "VkGraphicsComponent.h"
#include "VkPipelineBuilder.h"
#include <unordered_map>

class VkPipelineManager
{
  public:
	VkPipelineManager(VkGraphicsComponent &_gfx);
	~VkPipelineManager();


	VkPipelineManager()                            = delete;
	VkPipelineManager(const VkPipelineManager &) = delete;
	VkPipelineManager &operator=(const VkPipelineManager &) = delete;
	VkPipelineManager(VkPipelineManager &&)                 = delete;
	VkPipelineManager &operator=(VkPipelineManager &&) = delete;





	void AddPipeline(const VkPipelineParameterPack &para_pack, const VkRenderPass render_pass, const PipelineMetaInfo pipe_meta_info);
	void AddPipelineLayout(const PipelineLayoutMetaInfo& pipe_layout_info,const std::vector<VkDescriptorSetLayout>& des_layouts);


	[[nodiscard]] VkPipeline GetPipeline(const PipelineMetaInfo &meta_info) const;

	[[nodiscard]]VkPipelineLayout GetPipelineLayout(const PipelineLayoutMetaInfo &meta_info) const;
  private:
	VkGraphicsComponent &gfx;
	const VkDeviceManager &    device_manager;
	//const VkDescriptorManager &descriptor_manager;
  private:
	std::unordered_map<PipelineMetaInfo, VkPipeline> piplines;
	std::unordered_map<PipelineLayoutMetaInfo, VkPipelineLayout> pipline_layouts;

	VkShaderManager     shader_factory;
	VkPipelineBuilder pipeline_builder;

};
