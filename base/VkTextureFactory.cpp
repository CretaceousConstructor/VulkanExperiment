//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "VkTextureFactory.h"

VkTextureFactory::VkTextureFactory(VkGraphicsComponent &_gfx, const VkImageFactory &img_factory_, const VkBufferFactory &buffer_factory_) :
    gfx(_gfx), device_manager(gfx.DeviceMan()), window(gfx.Window()), img_factory(img_factory_), buffer_factory(buffer_factory_)
{


}

std::shared_ptr<VkTexture> VkTextureFactory::GetTexture(const std::string &image_path, VkFormat format_of_image, const SamplerParaPack &sampler_para_pack, VkImageLayout para_imageLayout) const
{
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	SamplerParaPack                        sam_para_pack{sampler_para_pack};
	if (true)
	{
		texture_image = InitKTXTexture(image_path, format_of_image, para_imageLayout, sam_para_pack);
	}
	else
	{
		//InitTexture(image_path, format_of_image, para_imageLayout,para_pack);
	}

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);

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

	sam_para_pack.sampler_CI.anisotropyEnable = VK_TRUE;
	sam_para_pack.sampler_CI.maxAnisotropy    = properties.limits.maxSamplerAnisotropy;
	//para_pack.sampler_CI.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
	sam_para_pack.sampler_CI.maxLod = (sam_para_pack.mip_count > 1) ? static_cast<float>(sam_para_pack.mip_count) : 0.0f;
	VkSampler texture_sampler   = InitSampler(sam_para_pack);

	return std::make_shared<VkTexture>(gfx, image_path, texture_image, texture_sampler, para_imageLayout);
}

//void VkTextureFactory::InitTexture(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout _image_layout,SamplerParaPack & sample_para_pack)const
//{
//	throw std::runtime_error("not implemented!");
//
//}

std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitKTXTexture(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, SamplerParaPack &sampler_para_pack_) const
{
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	const auto                             tex_name = image_path.substr(image_path.find_last_of("\\/") + 1, image_path.length());
	//========================================================================================================================
	ktxTexture *    ktxTexture;
	const ktxResult result = ktxTexture_CreateFromNamedFile(image_path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
	assert(result == KTX_SUCCESS);

	const ktx_uint8_t *const ktxTextureData = ktxTexture_GetData(ktxTexture);
	const ktx_size_t         ktxTextureSize = ktxTexture_GetSize(ktxTexture);
	//========================================================================================================================
	const VkExtent3D temp_image_extent{.width = ktxTexture->baseWidth, .height = ktxTexture->baseHeight, .depth = ktxTexture->baseDepth};

	constexpr bool forceLinearTiling = false;
	VkBool32       ktx_use_staging   = true;

	if (forceLinearTiling)
	{
		// Don't use linear if format is not supported for (linear) shader sampling
		// Get device properties for the requested texture format
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(device_manager.GetPhysicalDevice(), format_of_image, &formatProperties);
		ktx_use_staging = !(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
	}

	if (ktx_use_staging)
	{
		// Copy data to an optimal tiled image
		// This loads the texture data into a host local buffer that is copied to the optimal tiled image on the device

		// Create a host-visible staging buffer that contains the raw image data
		// This buffer will be the data source for copying texture data to the optimal tiled image on the device

		//========================================================================================================================
		VkBufferCI::StagingBuffer staging_para;
		auto staging_buffer = buffer_factory.ProduceBuffer(ktxTextureSize,staging_para);


		//VkBuffer       stagingBuffer;        //host visible memory
		//VkDeviceMemory stagingBufferMemory;

		//device_manager.CreateBufferAndBindToMemo(ktxTextureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());

		//创建完staging buffer以后，把cpu中的texture中的数据拷贝进staging buffer
		staging_buffer->MapMemory(0, ktxTextureSize, (void const *)ktxTextureData, ktxTextureSize);


		//void *data;
		////TODO: whether to use memReqs.size or just ktxTexureSize
		////VK_CHECK_RESULT(vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **) &data));
		//vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, ktxTextureSize, static_cast<VkMemoryMapFlags>(0), (void **) &data);
		//memcpy(data, ktxTextureData, static_cast<size_t>(ktxTextureSize));
		//vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);
		//========================================================================================================================


		//Setup buffer copy regions for each mip level
		//TODO:这里没有考虑layer的问题，
		std::vector<VkBufferImageCopy> bufferCopyRegions;

		for (uint32_t i = 0; i < ktxTexture->numLevels; i++)
		{
			// Calculate offset into staging buffer for the current mip level
			ktx_size_t     offset;
			KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
			assert(ret == KTX_SUCCESS);
			// Setup a buffer image copy structure for the current mip level
			VkBufferImageCopy bufferCopyRegion               = {};
			bufferCopyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel       = i;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount     = 1;
			bufferCopyRegion.imageExtent.width               = (ktxTexture->baseWidth) >> i;        //除法
			bufferCopyRegion.imageExtent.height              = (ktxTexture->baseHeight) >> i;
			bufferCopyRegion.imageExtent.depth               = 1;
			bufferCopyRegion.bufferOffset                    = offset;
			bufferCopyRegions.push_back(bufferCopyRegion);
		}

		TexImgParameterPack para_pack(tex_name, format_of_image, temp_image_extent);

		para_pack.default_image_CI.mipLevels   = ktxTexture->numLevels;
		para_pack.default_image_CI.arrayLayers = ktxTexture->numLayers;

		para_pack.default_image_view_CI.subresourceRange.levelCount = ktxTexture->numLevels;
		para_pack.default_image_view_CI.subresourceRange.layerCount = ktxTexture->numLayers;

		texture_image = std::dynamic_pointer_cast<VkGeneralPurposeImage>(img_factory.ProduceImage(para_pack));



		texture_image->CopyBufferToImage(staging_buffer->GetBuffer(), bufferCopyRegions, VkDeviceManager::CommandPoolType::transfor_command_pool);
		texture_image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image_layout_, VkDeviceManager::CommandPoolType::graphics_command_pool, ktxTexture->numLevels, ktxTexture->numLayers);


		//========================================================================================================================
		sampler_para_pack_.mip_count   = ktxTexture->numLevels;
		sampler_para_pack_.layer_count = ktxTexture->numLevels;


	}

	else
	{
		//TODO: handle situation when we can't use staging buffer
	}

	ktxTexture_Destroy(ktxTexture);

	//========================================================================================================================
	return texture_image;
}

VkSampler VkTextureFactory::InitSampler(const SamplerParaPack &para_pack) const
{
	VkSampler result;
	if (vkCreateSampler(device_manager.GetLogicalDevice(), &para_pack.sampler_CI, nullptr, &result) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
	return result;
}
