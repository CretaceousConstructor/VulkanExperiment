#include "VkShaderFactory.h"

using namespace Microsoft::WRL;

VkShaderFactory::VkShaderFactory(VkGraphicsComponent &gfx_) :
    gfx(gfx_),
    device_manager(gfx.DeviceMan())
{
	HRESULT hres;

	// Initialize DXC utils
	hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxc_utils.ReleaseAndGetAddressOf()));
	if (FAILED(hres))
	{
		throw std::runtime_error("Could not init DXC Utility");
	}

	// Initialize the DXC compiler
	hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxc_compiler.ReleaseAndGetAddressOf()));
	if (FAILED(hres))
	{
		throw std::runtime_error("Could not init DXC Compiler");
	}
}

std::shared_ptr<VkShaderWrapper> VkShaderFactory::GetShader(const std::string &path_, VkShaderStageFlagBits stage) const
{
	std::vector<char> hlsl_str = VkShaderFactory::ReadFile(path_);

	HRESULT         hres;
	const DxcBuffer src_buffer{
	    .Ptr      = hlsl_str.data(),
	    .Size     = static_cast<uint32_t>(hlsl_str.size()),
	    .Encoding = 0,
	};

	//GET FILE NAME
	const size_t start_pos = path_.find_last_of('/');
	//const auto   file_name = path_.substr(start_pos + 1, path_.size());
	const auto file_name = path_;

	//CONVERT TO WSTRING
	const int            wchars_num = MultiByteToWideChar(CP_UTF8, 0, file_name.c_str(), -1, nullptr, 0);
	std::vector<wchar_t> wstr(wchars_num);
	MultiByteToWideChar(CP_UTF8, 0, file_name.c_str(), -1, wstr.data(), wchars_num);
	std::vector<LPCWSTR> arguments;

	//COMPILATION ARGUMENTS
	arguments.push_back(L"-spirv");
	arguments.push_back(L"-fspv-target-env=vulkan1.3");
	arguments.push_back(L"-fspv-debug=vulkan-with-source");

	arguments.push_back(L"-T");        //target stage
	switch (stage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
			arguments.push_back(L"vs_6_0");
			break;
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			arguments.push_back(L"ps_6_0");
			break;
		default:
			throw std::runtime_error("Unknown shader stage!");
	}

	arguments.push_back(L"-E");        //entry point
	arguments.push_back(L"main");

	arguments.push_back(L"-HV");        //hlsl version
	arguments.push_back(L"2021");

	//arguments.push_back(DXC_ARG_DEBUG);        //-Zi选项会被-fspv-debug覆盖。
	//arguments.push_back(L"-Qembed_debug");
	//arguments.push_back(DXC_ARG_DEBUG_NAME_FOR_SOURCE);           

	arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);              //为了更好的调试体验
	arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);             //-WX
	arguments.push_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);        //-Zpc


	//put your shader name here
	arguments.push_back(wstr.data());

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

		throw std::runtime_error("compile error!");
	}

	VkShaderModuleCreateInfo shader_module_CI{};
	shader_module_CI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;



	//static std::vector<std::uint32_t> spriv_buffer;
	//spriv_buffer.resize(shader_obj->GetBufferSize() / sizeof(std::uint32_t));

	//for (size_t i = 0; i < spriv_buffer.size(); ++i)
	//{
	//	const std::uint32_t spv_uint = static_cast<std::uint32_t *>(shader_obj->GetBufferPointer())[i];        //
	//	spriv_buffer[i]              = spv_uint;
	//}
	//shader_module_CI.codeSize = shader_obj->GetBufferSize();
	//shader_module_CI.pCode    = spriv_buffer.data();

	//Create a Vulkan shader module from the compilation result
	shader_module_CI.codeSize = shader_obj->GetBufferSize();
	shader_module_CI.pCode    = static_cast<uint32_t *>(shader_obj->GetBufferPointer());

	VkShaderModule shader_module;
	VK_CHECK_RESULT(vkCreateShaderModule(device_manager.GetLogicalDevice(), &shader_module_CI, nullptr, &shader_module))

	return std::make_shared<VkShaderWrapper>(stage, path_, shader_module, gfx);
}

std::vector<char> VkShaderFactory::ReadFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open shader file!");
	}

	const auto        file_size = file.tellg();
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();

	return buffer;
}
