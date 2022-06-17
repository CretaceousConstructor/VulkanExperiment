#include "ShaderWrapper.h"

ShaderWrapper::ShaderWrapper(VkDeviceManager &_device_manager, std::string para_shader_file_name, VkShaderStageFlagBits para_shader_binding_stage) :
    device_manager(_device_manager),
    shader_file_name(para_shader_file_name),
    shader_binding_stage(para_shader_binding_stage)

{

	auto vertShaderCode             = ReadFile(shader_file_name);
	shader_module                   = CreateShaderModule(vertShaderCode);

	shader_stage_create_info        = VkPipelineShaderStageCreateInfo{};
	shader_stage_create_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_create_info.stage  = shader_binding_stage;
	shader_stage_create_info.module = shader_module;
	shader_stage_create_info.pName  = shader_enter_point.c_str();



}




ShaderWrapper::~ShaderWrapper()
{


	vkDestroyShaderModule(device_manager.GetLogicalDevice(), shader_module, nullptr);


}

VkPipelineShaderStageCreateInfo ShaderWrapper::GetVkPipelineShaderStageCreateInfo() const
{
	return shader_stage_create_info;
}

std::vector<char> ShaderWrapper::ReadFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	const size_t            fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

VkShaderModule ShaderWrapper::CreateShaderModule(std::vector<char> &code) const
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
