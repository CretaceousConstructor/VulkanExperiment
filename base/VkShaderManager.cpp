#include "VkShaderManager.h"

using namespace Microsoft::WRL;
VkShaderManager::VkShaderManager(VkGraphicsComponent &_gfx) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan())
{
	HRESULT hres;

	// Initialize DXC utils

	hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxc_utils.ReleaseAndGetAddressOf()));
	if (FAILED(hres))
	{
		throw std::runtime_error("Could not init DXC Library");
	}

	// Initialize the DXC compiler
	hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxc_compiler));
	if (FAILED(hres))
	{
		throw std::runtime_error("Could not init DXC Compiler");
	}
}

VkShaderManager::~VkShaderManager()
{
	for (const auto &shader : shaders_dict)
	{
		vkDestroyShaderModule(device_manager.GetLogicalDevice(), shader.second, nullptr);
	}
}

VkPipelineShaderStageCreateInfo VkShaderManager::GetShader(const ShaderMetaInfo &shader_meta_info)
{
	if (shaders_dict.contains(shader_meta_info))
	{
		return shader_stage_CI.at(shader_meta_info);
	}

	const std::vector<char> shader_code   = ReadFile(shader_meta_info.shader_path);
	VkShaderModule          shader_module = CreateShaderModule(shader_code);

	VkPipelineShaderStageCreateInfo shader_stage_create_info{};
	shader_stage_create_info        = VkPipelineShaderStageCreateInfo{};
	shader_stage_create_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_create_info.stage  = shader_meta_info.shader_binding_stage;
	shader_stage_create_info.module = shader_module;
	shader_stage_create_info.pName  = shader_entry_name.c_str();

	//TODO:
	//这里报错是因为没有实现hash函数
	shader_stage_CI.insert({shader_meta_info, shader_stage_create_info});
	shaders_dict.insert({shader_meta_info, shader_module});

	return shader_stage_create_info;
}

VkPipelineShaderStageCreateInfo VkShaderManager::GetShader(std::string filename_)
{
	std::vector<char> hlsl_str = ReadFile(filename_);

	HRESULT hres;
	const DxcBuffer src_buffer{
	    .Ptr      = hlsl_str.data(),
	    .Size     = static_cast<uint32_t>(hlsl_str.size()),
	    .Encoding = 0,
	};




	std::vector<LPCWSTR> arguments;
	arguments.push_back(L"your shader fucking names here");
	arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
	arguments.push_back(DXC_ARG_DEBUG); //-Zi
	arguments.push_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR); //-Zp

	arguments.push_back(L"-HV");        
	arguments.push_back(L"2021");        

	arguments.push_back(L"T");

	//switch (shader_stage)
	//{
	//		
	//	case:compute shader: push back(L"cs_6_0" );
	//	case:vertex shader:
	//		push back(cs_6_0);
	//	...
	//}


	//if (true)
	//{
	//throw std::runtime_error("need some function to specify shader staeges");
	//}


	arguments.push_back(L"-E");
	arguments.push_back(L"main");

	arguments.push_back(L"-spirv");
	arguments.push_back(L"-fspv-target-env=vulkan1.1");




	ComPtr<IDxcResult> compile_result;
	hres = dxc_compiler->Compile(&src_buffer, arguments.data(), (uint32_t) arguments.size(), nullptr, IID_PPV_ARGS(&compile_result));

	if (FAILED(hres))
	{
		throw std::runtime_error("failed to compile shader!");
	}

	ComPtr<IDxcBlob> shader_obj;
	compile_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_obj), nullptr);


	//Error Handling
	ComPtr<IDxcBlobUtf8> error_messages;
	compile_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(error_messages.GetAddressOf()), nullptr);
	if (error_messages && error_messages->GetStringLength() > 0)
	{
		auto str = std::string();
		str.resize(error_messages->GetBufferSize());
		for (size_t i = 0; i < str.size(); ++i)
		{
			str[i] = static_cast<char const *>(error_messages->GetBufferPointer())[i];

		}
		MessageBoxA(nullptr, str.c_str(), "DXC Compiler Error", MB_OK);
	}






	std::vector<std::uint32_t> spriv_buffer;
	spriv_buffer.resize(shader_obj->GetBufferSize() / sizeof(std::uint32_t));

	// Create a Vulkan shader module from the compilation result
	VkShaderModuleCreateInfo shader_module_CI{};
	shader_module_CI.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_CI.codeSize = spriv_buffer.size();
	shader_module_CI.pCode    = spriv_buffer.data();
	VkShaderModule shaderModule;
	VK_CHECK_RESULT(vkCreateShaderModule(device_manager.GetLogicalDevice(), &shader_module_CI, nullptr, &shaderModule))




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
