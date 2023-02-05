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

VkShaderWrapper::VkShaderWrapper(VkShaderStageFlagBits binding_stage_, const std::string &shader_path_, VkShaderModule shader_module_, VkGraphicsComponent &gfx_) :
    device_manager(gfx_.DeviceMan()), shader_module(shader_module_), binding_stage(binding_stage_), shader_path(shader_path_)
{
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

	VK_CHECK_RESULT(vkCreateShaderModule(device_manager.GetLogicalDevice(), &create_info, nullptr, &shader_module))



	//const std::string pathName = "D:/CS/ComputerGraphics/vulkan/WindowsProject1/data/shaderdebuginfo/blob.dxbc"; // path name is in UTF-8
	//auto func_ptr_to_vkSetDebugUtilsObjectTagEXTo = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(vkGetDeviceProcAddr(device_manager.GetLogicalDevice(), "vkSetDebugUtilsObjectTagEXT"));
	//// Both EXT_debug_marker and EXT_debug_utils can be used, this example uses
	//// EXT_debug_utils as EXT_debug_marker is deprecated
	//VkDebugUtilsObjectTagInfoEXT tagInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT};
	//tagInfo.objectType = VK_OBJECT_TYPE_SHADER_MODULE;
	//tagInfo.objectHandle = reinterpret_cast<uint64_t>(shader_module);
	//// tag value in renderdoc_app.h
	////RENDERDOC_ShaderDebugMagicValue_truncated 
	//tagInfo.tagName = 0x48656670eab25520ULL;
	//tagInfo.pTag = pathName.c_str();
	//tagInfo.tagSize = pathName.length();

	////vkSetDebugUtilsObjectTagEXT(device_manager.GetLogicalDevice(), &tagInfo);
	//func_ptr_to_vkSetDebugUtilsObjectTagEXTo(device_manager.GetLogicalDevice(), &tagInfo);


}

VkPipelineShaderStageCreateInfo VkShaderWrapper::GetShaderStageCI() const
{
	VkPipelineShaderStageCreateInfo shader_stage_create_info{};
	shader_stage_create_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_create_info.stage  = binding_stage;
	shader_stage_create_info.module = shader_module;
	shader_stage_create_info.pName  = entry_point.c_str();

	return shader_stage_create_info;
}
