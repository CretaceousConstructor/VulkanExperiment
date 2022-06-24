#include "VkPipelineWrapper.h"

VkPipelineWrapper::VkPipelineWrapper(VkGraphicsComponent& _gfx) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan())
{
}

VkPipelineWrapper::~VkPipelineWrapper()
{
	vkDestroyPipeline(device_manager.GetLogicalDevice(), pipeline, nullptr);
}

void VkPipelineWrapper::AddShaders(const VkShaderManager::ShaderInfo &shader_info)
{

	shaders.emplace_back(device_manager, shader_info.name, shader_info.shader_binding_stage);

}

std::vector<VkPipelineShaderStageCreateInfo> VkPipelineWrapper::GetShaderStageCIVec() const
{
	std::vector<VkPipelineShaderStageCreateInfo> result;
	for (const auto &shader : shaders)
	{
		result.push_back(shader.GetVkPipelineShaderStageCreateInfo());
	}
	return result;
}

VkPipeline VkPipelineWrapper::GetPipeline() const
{
	return pipeline;
}
