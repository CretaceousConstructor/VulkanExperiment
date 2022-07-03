#pragma once
#include "VkGraphicsComponent.h"
#include "VkPipelineWrapper.h"
#include "VkRenderpassWrapper.h"
#include "VkPipelineParameterPack.h"
#include "VkShaderManager.h"
#include <memory>

class VkPipelineBuilder
{
  public:
	VkPipelineBuilder(VkGraphicsComponent& _gfx,VkShaderManager& _shader_factory);
	~VkPipelineBuilder() = default;

	VkPipelineBuilder()                          = delete;
	VkPipelineBuilder(const VkPipelineBuilder &) = delete;
	VkPipelineBuilder &operator=(const VkPipelineBuilder &) = delete;
	VkPipelineBuilder(VkPipelineBuilder &&)                 = delete;
	VkPipelineBuilder &operator=(VkPipelineBuilder &&) = delete;

  public:
	VkPipeline BuildPipeline(const VkPipelineParameterPack &para_pack, const VkRenderPass render_pass, VkPipelineLayout pipeline_layout, const PipelineMetaInfo &pipe_meta_info) const;
	VkPipeline BuildPipeline(const VkPipelineParameterPack &para_pack, const uint32_t supass) const;

  private:
	VkGraphicsComponent &gfx;
	const VkDeviceManager &   device_manager;
	VkShaderManager &      shader_factory;


};