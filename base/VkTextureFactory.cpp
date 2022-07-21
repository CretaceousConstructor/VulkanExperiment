//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "VkTextureFactory.h"

VkTextureFactory::VkTextureFactory(VkGraphicsComponent &_gfx, const VkImageFactory &img_factory_, const VkBufferFactory &buffer_factory_) :
    gfx(_gfx), device_manager(gfx.DeviceMan()), window(gfx.Window()), img_factory(img_factory_), buffer_factory(buffer_factory_)
{
}

std::shared_ptr<VkTexture> VkTextureFactory::ProduceTexture(const std::string &image_path, VkFormat format_of_image_, const SamplerPP &sampler_pp_, VkImageLayout image_layout_) const
{
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	SamplerPP                              sam_para_pack{sampler_pp_};

	const size_t pos            = image_path.find_last_of('.');
	const auto   file_extension = image_path.substr(pos + 1, image_path.size());

	if (std::string("ktx") == file_extension) // ktx.h can process both hdr and ldr imges.
	{
		texture_image = InitKTXImgFromFile(image_path, format_of_image_, image_layout_, sam_para_pack);
	}
	else if (stbi_is_hdr(image_path.c_str()))
	{
		texture_image = InitHdrImgFromFile(image_path, format_of_image_, image_layout_, sam_para_pack);
	}
	else
	{
		texture_image = InitOtherFormatImgFromFile(image_path, format_of_image_, image_layout_, sam_para_pack);
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
	VkSampler texture_sampler       = InitSampler(sam_para_pack);

	return std::make_shared<VkTexture>(gfx, image_path, texture_image, texture_sampler, image_layout_);
}

std::shared_ptr<VkTexture> VkTextureFactory::ProduceTexture(const TexturePP &texture_pp, const SamplerPP &sampler_para_pack, VkImageLayout para_imageLayout) const
{
	std::string                            image_path = "null";
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	SamplerPP                              sam_para_pack{sampler_para_pack};
	InitImgFromBuffer<unsigned char>(texture_pp, para_imageLayout, sam_para_pack);

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
	sam_para_pack.sampler_CI.maxAnisotropy    = 1.f;

	sam_para_pack.sampler_CI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sam_para_pack.sampler_CI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sam_para_pack.sampler_CI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	//para_pack.sampler_CI.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
	sam_para_pack.sampler_CI.maxLod = (sam_para_pack.mip_count > 1) ? static_cast<float>(sam_para_pack.mip_count) : 0.0f;
	VkSampler texture_sampler       = InitSampler(sam_para_pack);

	return std::make_shared<VkTexture>(gfx, image_path, texture_image, texture_sampler, para_imageLayout);
}

std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitOtherFormatImgFromFile(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, SamplerPP &sam_pack) const
{
	//TODO: mipmap info and layer info
	// Get the image data from the stbi loader
	int            width, height, nrChannels;
	unsigned char *data = stbi_load(image_path.c_str(), &width, &height, &nrChannels, 0);
	if (nullptr == data)
	{
		throw std::runtime_error("failed to load image using stb!");
	}

	unsigned char *buffer        = nullptr;
	bool           delete_buffer = false;
	// If it's an RGB image (3 components), we convert it to RGBA as most Vulkan implementations don't support 8-Bit RGB-only formats
	if (nrChannels == 3)
	{
		buffer              = new unsigned char[width * height * 4];
		unsigned char *rgba = buffer;
		//unsigned char* rgb = &glTFImage.image[0];
		const unsigned char *rgb = data;
		for (size_t i = 0; i < width * height; ++i)
		{
			memcpy(rgba, rgb, sizeof(unsigned char) * 3);
			rgba[3] = 0xff;
			rgba += 4;
			rgb += 3;
		}
		delete_buffer = true;
	}
	else
	{
		buffer = data;
	}

	assert(width > 0 && height > 0);

	const TexturePP texture_pp{buffer, format_of_image, uint32_t(width), uint32_t(height)};
	auto            images = InitImgFromBuffer<unsigned char>(texture_pp, image_layout_, sam_pack);

	if (delete_buffer)
	{
		delete[] buffer;
	}
	stbi_image_free(data);
	return images;
}

std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitHdrImgFromFile(const std::string &image_path, const VkFormat format_of_image_, const VkImageLayout image_layout_, SamplerPP &sampler_pp_) const
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;

	//Get the image data from the stbi loader
	float *data = stbi_loadf(image_path.c_str(), &width, &height, &nrComponents, 0);

	assert(width > 0 && height > 0);
	if (nullptr == data)
	{
		throw std::runtime_error("failed to load HDR image!");
	}

	float *buffer        = nullptr;
	bool   delete_buffer = false;
	// If it's an RGB image (3 components), we convert it to RGBA as most Vulkan implementations don't support 8-Bit RGB-only formats
	if (nrComponents == 3)
	{
		buffer            = new float[width * height * 4];
		float *      rgba = buffer;
		const float *rgb  = data;
		for (size_t i = 0; i < width * height; ++i)
		{
			memcpy(rgba, rgb, sizeof(float) * 3);
			rgba[3] = 1.f;
			rgba += 4;
			rgb += 3;
		}
		delete_buffer = true;
	}
	else
	{
		buffer = data;
	}

	const TexturePP texture_pp{buffer, format_of_image_, uint32_t(width), uint32_t(height)};
	auto            images = InitImgFromBuffer<float>(texture_pp, image_layout_, sampler_pp_);

	if (delete_buffer)
	{
		delete[] buffer;
	}
	stbi_image_free(data);
	return images;
}

//This function should be move to other class
std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitKTXImgFromFile(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, SamplerPP &sampler_para_pack_) const
{
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	const auto                             tex_name = image_path.substr(image_path.find_last_of("\\/") + 1, image_path.length());
	//========================================================================================================================

	ktxTexture *ktxTexture;
	//ktxTexture has a   ktx_uint32_t glFormat field;

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
		auto                      staging_buffer = buffer_factory.ProduceBuffer(ktxTextureSize, staging_para);

		//VkBuffer       stagingBuffer;        //host visible memory
		//VkDeviceMemory stagingBufferMemory;

		//device_manager.CreateBufferAndBindToMemo(ktxTextureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());

		//������staging buffer�Ժ󣬰�cpu�е�texture�е����ݿ�����staging buffer
		staging_buffer->CopyTo((void const *) ktxTextureData, ktxTextureSize);

		//void *data;
		////TODO: whether to use memReqs.size or just ktxTexureSize
		////VK_CHECK_RESULT(vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **) &data));
		//vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, ktxTextureSize, static_cast<VkMemoryMapFlags>(0), (void **) &data);
		//memcpy(data, ktxTextureData, static_cast<size_t>(ktxTextureSize));
		//vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);
		//========================================================================================================================

		//Setup buffer copy regions for each mip level
		//TODO:����û�п���layer�����⣬
		std::vector<VkBufferImageCopy> bufferCopyRegions;

		for (uint32_t i = 0; i < ktxTexture->numLevels; i++)
		{
			// Calculate offset into staging buffer for the current mip level
			ktx_size_t     offset;
			KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
			assert(ret == KTX_SUCCESS);
			// Setup a buffer image copy structure for the current mip level
			VkBufferImageCopy bufferCopyRegion{};
			bufferCopyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel       = i;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount     = 1;
			bufferCopyRegion.imageExtent.width               = (ktxTexture->baseWidth) >> i;        //����
			bufferCopyRegion.imageExtent.height              = (ktxTexture->baseHeight) >> i;
			bufferCopyRegion.imageExtent.depth               = 1;
			bufferCopyRegion.bufferOffset                    = offset;
			bufferCopyRegions.push_back(bufferCopyRegion);
		}

		TextureImgPP para_pack(tex_name, format_of_image, temp_image_extent);

		para_pack.default_image_CI.mipLevels   = ktxTexture->numLevels;
		para_pack.default_image_CI.arrayLayers = ktxTexture->numLayers;

		para_pack.default_image_view_CI.subresourceRange.levelCount = ktxTexture->numLevels;
		para_pack.default_image_view_CI.subresourceRange.layerCount = ktxTexture->numLayers;

		texture_image = std::dynamic_pointer_cast<VkGeneralPurposeImage>(img_factory.ProduceImage(para_pack));

		texture_image->CopyBufferToImage(staging_buffer->GetRawBuffer(), bufferCopyRegions, VkDeviceManager::CommandPoolType::transfor_command_pool);
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

VkSampler VkTextureFactory::InitSampler(const SamplerPP &para_pack) const
{
	VkSampler result;

	if (vkCreateSampler(device_manager.GetLogicalDevice(), &para_pack.sampler_CI, nullptr, &result) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
	return result;
}
