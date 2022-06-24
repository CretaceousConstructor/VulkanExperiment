#include "VkShaderManager.h"

VkShaderManager::VkShaderManager(VkGraphicsComponent &_gfx) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan())
{
	
}

	VkShaderManager::~VkShaderManager()
{

	for (const auto& shader :shaders_dict)
	{
		vkDestroyShaderModule(device_manager.GetLogicalDevice(), shader.second, nullptr);
	}

}




VkPipelineShaderStageCreateInfo VkShaderManager::GetShader(const ShaderMetaInfo &shader_meta_info) const
{


	if (shaders_dict.contains(shader_meta_info))
	{
		return shader_stage_CI.at(shader_meta_info);
	}

	const std::vector<char> shader_code = ReadFile(shader_meta_info.shader_path);
	const VkShaderModule shader_module       = CreateShaderModule(shader_code);

	//TODO:
	//这里报错是因为没有实现hash函数
	//shaders_dict.insert({shader_meta_info, shader_module});
	VkPipelineShaderStageCreateInfo  shader_stage_create_info{};
	shader_stage_create_info        = VkPipelineShaderStageCreateInfo{};
	shader_stage_create_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_create_info.stage  = shader_meta_info.shader_binding_stage;
	shader_stage_create_info.module = shader_module;
	shader_stage_create_info.pName  = shader_entry_name.c_str();

	//TODO:
	//这里报错是因为没有实现hash函数
	//shader_stage_CI.insert({shader_meta_info,shader_stage_create_info});
	return shader_stage_create_info;

}


std::vector<char> VkShaderManager::ReadFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	const size_t      fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}



VkShaderModule VkShaderManager::CreateShaderModule(const std::vector<char> &code) const
{
	//typedef struct VkShaderModuleCreateInfo {
	//	VkStructureType              sType;
	//	const void* pNext;
	//	VkShaderModuleCreateFlags    flags;
	//	size_t                       codeSize;
	//	const uint32_t* pCode;
	//} VkShaderModuleCreateInfo;
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode    = reinterpret_cast<const uint32_t *>(code.data());

	//Lucky for us, the data is stored in an std::vector where the default allocator already ensures that the data satisfies the worst case ALIGNMENT requirements.

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device_manager.GetLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}
