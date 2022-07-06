#pragma once
#include "VkGraphicsComponent.h"
#include "VkPipelineParameterPack.h"
#include "VkPipelineWrapper.h"
#include "VkRenderpassWrapper.h"
#include "VkShaderManager.h"
#include <memory>

class VkPipelineBuilder
{
  public:
	VkPipelineBuilder(VkGraphicsComponent &_gfx, VkShaderManager &_shader_factory);
	~VkPipelineBuilder() = default;

	VkPipelineBuilder()                          = delete;
	VkPipelineBuilder(const VkPipelineBuilder &) = delete;
	VkPipelineBuilder &operator=(const VkPipelineBuilder &) = delete;
	VkPipelineBuilder(VkPipelineBuilder &&)                 = delete;
	VkPipelineBuilder &operator=(VkPipelineBuilder &&) = delete;

  public:
	VkPipeline BuildPipeline(const VkPipelineParameterPack &para_pack, VkPipelineLayout pipeline_layout, const PipelineMetaInfo &pipe_meta_info) const;
	[[nodiscard]] VkPipeline BuildPipeline(const VkPipelineParameterPack &para_pack) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
	VkShaderManager &      shader_factory;
};