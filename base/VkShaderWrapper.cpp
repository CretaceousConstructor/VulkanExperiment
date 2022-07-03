#include "VkShaderWrapper.h"

VkShaderWrapper::VkShaderWrapper(const ParameterPack &para_pack, VkGraphicsComponent &gfx_) :
    device_manager(gfx_.DeviceMan()), binding_stage(para_pack.binding_stage), entry_point(para_pack.entry_point)

{




	const auto shader_code = ReadFile(para_pack.shader_path);

	CreateShaderModule(shader_code);
}

VkShaderWrapper::VkShaderWrapper(VkShaderStageFlagBits binding_stage, std::string shader_path_, VkGraphicsComponent &gfx_) :
    device_manager(gfx_.DeviceMan()), binding_stage(binding_stage), shader_path(std::move(shader_path_))

{
	const auto shader_code = ReadFile(shader_path);
	CreateShaderModule(shader_code);
}

VkShaderWrapper::~VkShaderWrapper()
{
	vkDestroyShaderModule(device_manager.GetLogicalDevice(), shader_module, nullptr);
}

std::vector<char> VkShaderWrapper::ReadFile(const std::string &filename)
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

void VkShaderWrapper::CreateShaderModule(const std::vector<char> &code)
{
	//typedef struct VkShaderModuleCreateInfo {
	//	VkStructureType              sType;
	//	const void* pNext;
	//	VkShaderModuleCreateFlags    flags;
	//	size_t                       codeSize;
	//	const uint32_t* pCode;
	//} VkShaderModuleCreateInfo;
	VkShaderModuleCreateInfo create_info{};
	create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = code.size();
	create_info.pCode    = reinterpret_cast<const uint32_t *>(code.data());
	//Lucky for us, the data is stored in an std::vector where the default allocator already ensures that the data satisfies the worst case ALIGNMENT requirements.

	if (vkCreateShaderModule(device_manager.GetLogicalDevice(), &create_info, nullptr, &shader_module) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}
}

VkPipelineShaderStageCreateInfo VkShaderWrapper::GetShaderStageCI() const
{
	VkPipelineShaderStageCreateInfo shader_stage_create_info{};
	shader_stage_create_info        = VkPipelineShaderStageCreateInfo{};
	shader_stage_create_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_create_info.stage  = binding_stage;
	shader_stage_create_info.module = shader_module;
	shader_stage_create_info.pName  = entry_point.c_str();

	return shader_stage_create_info;
}
