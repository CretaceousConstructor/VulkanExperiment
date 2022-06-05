#include "VkPipelineWrapper.h"

VkPipelineWrapper::VkPipelineWrapper(VkDeviceManager &_device_manager)
	:
    device_manager(_device_manager)
{


}


void VkPipelineWrapper::AddShaders(std::vector<ShaderManager::ShaderInfo> shader_infos)
{

	for (const auto& info: shader_infos)
	{
		shaders.emplace_back(device_manager, info.name, info.shader_binding_stage);
		shader_stages_create_info.push_back(shaders.back().GetVkPipelineShaderStageCreateInfo());
	}
		
}

