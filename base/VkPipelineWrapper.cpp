#include "VkPipelineWrapper.h"

VkPipelineWrapper::VkPipelineWrapper(VkDeviceManager &_device_manager) :
    device_manager(_device_manager)
{
}

VkPipelineWrapper::~VkPipelineWrapper()
{
	vkDestroyPipeline(device_manager.GetLogicalDevice(), pipeline, nullptr);
}

void VkPipelineWrapper::AddShaders(const ShaderWrapper::ShaderInfo &shader_info)
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
