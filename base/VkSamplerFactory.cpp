#include "VkSamplerFactory.h"

VkSamplerCreateInfo SamplerCI::PopulateTexSamplerCI()
{
	//typedef struct VkSamplerCreateInfo {
	//    VkStructureType         sType;
	//    const void*             pNext;
	//    VkSamplerCreateFlags    flags;
	//    VkFilter                magFilter;
	//    VkFilter                minFilter;
	//    VkSamplerMipmapMode     mipmapMode;
	//    VkSamplerAddressMode    addressModeU;
	//    VkSamplerAddressMode    addressModeV;
	//    VkSamplerAddressMode    addressModeW;

	//    float                   mipLodBias;
	//    VkBool32                anisotropyEnable;
	//    float                   maxAnisotropy;
	//    VkBool32                compareEnable;
	//    VkCompareOp             compareOp;
	//    float                   minLod;
	//    float                   maxLod;
	//    VkBorderColor           borderColor;
	//    VkBool32                unnormalizedCoordinates;
	//} VkSamplerCreateInfo;

	VkSamplerCreateInfo sampler_CI{};
	sampler_CI.sType      = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_CI.pNext      = nullptr;
	sampler_CI.flags      = 0;
	sampler_CI.magFilter  = VK_FILTER_LINEAR;
	sampler_CI.minFilter  = VK_FILTER_LINEAR;
	sampler_CI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	sampler_CI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_CI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_CI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	//VkPhysicalDeviceProperties properties{};
	//vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);

	sampler_CI.mipLodBias       = 0.f;
	sampler_CI.anisotropyEnable = VK_FALSE;
	//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
	sampler_CI.maxAnisotropy    = 0.f;

	sampler_CI.compareEnable = VK_FALSE;
	sampler_CI.compareOp     = VK_COMPARE_OP_ALWAYS;

	sampler_CI.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	sampler_CI.unnormalizedCoordinates = VK_FALSE;

	sampler_CI.minLod = 0.0f;
	sampler_CI.maxLod = 1.0f;
	//samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
	return sampler_CI;
}





VkSamplerCreateInfo SamplerCI::PopulateCubeTexSamplerCI()
{
	//typedef struct VkSamplerCreateInfo {
	//    VkStructureType         sType;
	//    const void*             pNext;
	//    VkSamplerCreateFlags    flags;
	//    VkFilter                magFilter;
	//    VkFilter                minFilter;
	//    VkSamplerMipmapMode     mipmapMode;
	//    VkSamplerAddressMode    addressModeU;
	//    VkSamplerAddressMode    addressModeV;
	//    VkSamplerAddressMode    addressModeW;

	//    float                   mipLodBias;
	//    VkBool32                anisotropyEnable;
	//    float                   maxAnisotropy;
	//    VkBool32                compareEnable;
	//    VkCompareOp             compareOp;
	//    float                   minLod;
	//    float                   maxLod;
	//    VkBorderColor           borderColor;
	//    VkBool32                unnormalizedCoordinates;
	//} VkSamplerCreateInfo;

	VkSamplerCreateInfo sampler_CI{};
	sampler_CI.sType      = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_CI.pNext      = nullptr;
	sampler_CI.flags      = 0;
	sampler_CI.magFilter  = VK_FILTER_LINEAR;
	sampler_CI.minFilter  = VK_FILTER_LINEAR;
	sampler_CI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	sampler_CI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_CI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_CI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	sampler_CI.mipLodBias       = 0.f;
	//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
	sampler_CI.anisotropyEnable = VK_FALSE;
	sampler_CI.maxAnisotropy    = 1.f;

	sampler_CI.compareEnable = VK_FALSE;
	sampler_CI.compareOp     = VK_COMPARE_OP_NEVER;
	//VkPhysicalDeviceProperties properties{};
	//vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);

	sampler_CI.borderColor             = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler_CI.unnormalizedCoordinates = VK_FALSE;

	sampler_CI.minLod = 0.0f;
	sampler_CI.maxLod = 1.0f;
	//samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
	return sampler_CI;
}





















VkSamplerFactory::VkSamplerFactory(VkGraphicsComponent &gfx_)

    :
    device_manager(gfx_.DeviceMan())
{
}

VkSamplerFactory::~VkSamplerFactory()
{
	for (const auto &pair : samplers_unsafe)
	{
		for (uint32_t i = 0; i < pair.second; i++)
		{
			vkDestroySampler(device_manager.GetLogicalDevice(), pair.first, nullptr);
		}
	}
}

VkSampler VkSamplerFactory::ProduceSamplerUnsafe(const VkSamplerCreateInfo &sampler_CI)
{
	VkSampler sampler;
	VK_CHECK_RESULT(vkCreateSampler(device_manager.GetLogicalDevice(), &sampler_CI, nullptr, &sampler))

	if (samplers_unsafe.contains(sampler))
	{
		++samplers_unsafe.at(sampler);
	}
	else
	{
		samplers_unsafe.insert(std::pair{sampler, 1});
	}

	return sampler;
}

void VkSamplerFactory::DestroySamplerUnsafe(VkSampler sampler)
{
	auto &amount = samplers_unsafe.at(sampler);
	assert(amount != 0);
	if (0 == --amount)
	{
		vkDestroySampler(device_manager.GetLogicalDevice(), sampler, nullptr);
	}
}
