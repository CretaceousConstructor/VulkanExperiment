//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "VkTextureFactory.h"

VkTextureFactory::VkTextureFactory(
    VkGraphicsComponent &  gfx_,
    const VkImageFactory & img_factory_,
    const VkBufferFactory &buffer_factory_,
    VkSamplerFactory &     sampler_factory_,
    VkImgViewFactory &     image_view_factory_) :
    gfx(gfx_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), window(gfx.Window()), img_factory(img_factory_), buffer_factory(buffer_factory_), sampler_factory(sampler_factory_), image_view_factory(image_view_factory_)
{
}

void VkTextureFactory::ProcessViewAndSampler(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, VkTexture &result_tex, VkImageBase &img) const
{
	if (img_view_CI_)
	{
		img_view_CI_->image        = img.GetImage();
		const VkImageView img_view = image_view_factory.ProduceImgViewUnsafe(img_view_CI_.value());
		result_tex.tex_image_view  = img_view;
	}
	else
	{
		result_tex.tex_image_view = nullptr;
	}
	//VkPhysicalDeviceProperties properties{};
	//vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);

	//sam_para_pack.sampler_CI.anisotropyEnable = VK_TRUE;
	//sam_para_pack.sampler_CI.maxAnisotropy    = 1.f;

	//sam_para_pack.sampler_CI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//sam_para_pack.sampler_CI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//sam_para_pack.sampler_CI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	//para_pack.sampler_CI.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
	//sam_para_pack.sampler_CI.maxLod = (sam_para_pack.mip_count > 1) ? static_cast<float>(sam_para_pack.mip_count) : 0.0f;
	//VkSampler texture_sampler       = InitSampler(sam_para_pack);
	//para_pack.default_image_view_CI.subresourceRange.levelCount = ktxTexture->numLevels;
	//para_pack.default_image_view_CI.subresourceRange.layerCount = ktxTexture->numLayers;

	//VkPhysicalDeviceProperties properties{};
	//vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);
	//sam_para_pack.sampler_CI.anisotropyEnable = VK_TRUE;
	//sam_para_pack.sampler_CI.maxAnisotropy    = properties.limits.maxSamplerAnisotropy;
	////para_pack.sampler_CI.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
	//sam_para_pack.sampler_CI.maxLod = (sam_para_pack.mip_count > 1) ? static_cast<float>(sam_para_pack.mip_count) : 0.0f;
	//VkSampler texture_sampler       = InitSampler(sam_para_pack);

	if (sampler_CI_)
	{
		const VkSampler texture_sampler = sampler_factory.ProduceSamplerUnsafe(sampler_CI_.value());
		result_tex.tex_sampler          = texture_sampler;
	}
	else
	{
		result_tex.tex_sampler = nullptr;
	}

	result_tex.SetCISamplerAndImgView(sampler_CI_, img_view_CI_);
}

//produce a texture without actual contents in it.
std::shared_ptr<VkTexture> VkTextureFactory::ProduceUnfilledTexture(const ImagePP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_) const
{
	std::string image_path = "null";
	auto        tex_img    = std::dynamic_pointer_cast<VkGeneralPurposeImage>(img_factory.ProduceImage(texture_img_PP));
	auto        result     = std::make_shared<VkTexture>(gfx, image_path, tex_img, texture_img_PP.default_layout_right_aft_creation);

	ProcessViewAndSampler(sampler_CI_, img_view_CI_, *result, *tex_img);

	return result;
}

std::vector<std::shared_ptr<VkTexture>> VkTextureFactory::ProduceEmptyTextureArray(const ImagePP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, uint32_t bundle_size) const
{
	std::vector<std::shared_ptr<VkTexture>> result;

	for (uint32_t i = 0; i < bundle_size; i++)
	{
		result.push_back(ProduceUnfilledTexture(texture_img_PP, sampler_CI_, img_view_CI_));
	}

	return result;
}

std::vector<std::shared_ptr<VkTexture>> VkTextureFactory::ProduceEmptyTextureArray(const SwapchainImgPP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_) const
{
	std::vector<std::shared_ptr<VkTexture>> result;
	const std::vector<VkImage> &            swap_images = swapchain_manager.GetSwapChainImages();

	for (uint32_t i = 0; i < swap_images.size(); i++)
	{
		std::string image_path = "null";

		const std::shared_ptr<ImagePP> para_pack_ptr = texture_img_PP.Clone();
		para_pack_ptr->default_image_format          = swapchain_manager.GetSwapChainImageFormat();
		para_pack_ptr->default_image_extent          = swapchain_manager.GetSwapChainImageExtent();

		auto tex_img = std::make_shared<VkSwapchainImage>(gfx, swap_images[i], para_pack_ptr);

		//tex_img->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack_ptr->default_final_layout, VkDeviceManager::CommandPoolType::graphics_command_pool, para_pack_ptr->default_image_CI.mipLevels, para_pack_ptr->default_image_CI.arrayLayers);

		const auto tex = std::make_shared<VkTexture>(gfx, image_path, tex_img, texture_img_PP.default_layout_right_aft_creation);

		ProcessViewAndSampler(sampler_CI_, img_view_CI_, *tex, *tex_img);

		result.push_back(tex);
	}

	return result;
}

void VkTextureFactory::ResetTexSampler(VkSamplerCreateInfo sampler_CI_, VkTexture &result_tex) const
{
	const VkSampler texture_sampler = sampler_factory.ProduceSamplerUnsafe(sampler_CI_);
	result_tex.tex_sampler          = texture_sampler;
	result_tex.SetSamplerCI(sampler_CI_);
	return;
}

void VkTextureFactory::ResetTexSampler(VkSamplerCreateInfo sampler_CI_, VkTexture::TexturePtrBundle &tex_ptr_bundle) const
{
	for (auto& tex : tex_ptr_bundle)
	{
		ResetTexSampler(sampler_CI_, *tex);
	}
}

void VkTextureFactory::ResetTexImgView(VkImageViewCreateInfo img_view_CI_, VkTexture &result_tex) const
{
	img_view_CI_.image         = result_tex.GetTextureRawImage();
	VkImageView img_view = image_view_factory.ProduceImgViewUnsafe(img_view_CI_);
	result_tex.tex_image_view  = img_view;
	result_tex.SetImgViewCI(img_view_CI_);
	return;
}

std::shared_ptr<VkTexture> VkTextureFactory::ProduceTextureFromBuffer(const ImagePP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, const void *const data, VkImageLayout image_layout_) const
{
	std::string                            image_path = "null";
	std::shared_ptr<VkGeneralPurposeImage> tex_img    = InitImgFromHostBuffer<unsigned char>(texture_img_PP, data, image_layout_);

	auto result = std::make_shared<VkTexture>(gfx, image_path, tex_img, image_layout_);

	ProcessViewAndSampler(sampler_CI_, img_view_CI_, *result, *tex_img);

	return result;
}

std::shared_ptr<VkTexture> VkTextureFactory::ProduceTextureFromImgPath(const std::string &image_path, VkFormat format_of_image_, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, VkImageLayout image_layout_) const
{
	std::shared_ptr<VkGeneralPurposeImage> tex_img;

	const auto file_extension = Vk::GetFileExtensionName(image_path);
	// ktx.h can process both hdr and ldr imges.
	if (std::string("ktx") == file_extension)
	{
		InfoFromFile info_from_file{};
		tex_img = InitKTXImgFromFile(image_path, format_of_image_, image_layout_, info_from_file);
		if (img_view_CI_)
		{
			img_view_CI_->subresourceRange.levelCount = info_from_file.numLevels;
			img_view_CI_->subresourceRange.layerCount = info_from_file.numArrayLayers;
		}

		if (sampler_CI_)
		{
			sampler_CI_->maxLod = static_cast<float>(info_from_file.numLevels);
		}
	}

	//TODO: Non-ktx texture mipmap generation
	//HDR images
	else if (stbi_is_hdr(image_path.c_str()))
	{
		tex_img = InitHdrImgFromFile(image_path, format_of_image_, image_layout_);
	}
	//LDR images
	else
	{
		tex_img = InitOtherFormatImgFromFile(image_path, format_of_image_, image_layout_);
	}

	auto result = std::make_shared<VkTexture>(gfx, image_path, tex_img, image_layout_);

	ProcessViewAndSampler(sampler_CI_, img_view_CI_, *result, *tex_img);

	return result;
}

std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitOtherFormatImgFromFile(const std::string &image_path, const VkFormat format_of_image_, const VkImageLayout image_layout_) const
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

	const TextureImgPP tex_img_pp{image_path, format_of_image_, VkExtent3D(width, height, 1)};
	auto               image = InitImgFromHostBuffer<unsigned char>(tex_img_pp, buffer, image_layout_);

	if (delete_buffer)
	{
		delete[] buffer;
	}
	stbi_image_free(data);
	return image;
}

std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitHdrImgFromFile(const std::string &image_path, const VkFormat format_of_image_, const VkImageLayout image_layout_) const
{
	//stbi_set_flip_vertically_on_load(true);
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

	TextureImgPP tex_img_pp{image_path, format_of_image_, VkExtent3D(width, height, 1)};
	auto         image = InitImgFromHostBuffer<float>(tex_img_pp, buffer, image_layout_);

	if (delete_buffer)
	{
		delete[] buffer;
	}
	stbi_image_free(data);
	return image;
}

//This function should be move to other class
std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitKTXImgFromFile(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, InfoFromFile &info_from_file) const
{
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	const auto                             tex_name = image_path.substr(image_path.find_last_of("\\/") + 1, image_path.length());
	//========================================================================================================================
	ktxTexture *ktxTexture;
	//ktxTexture struct has a ktx_uint32_t glFormat field representing the image's format

	const ktxResult result = ktxTexture_CreateFromNamedFile(image_path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);

	assert(result == KTX_SUCCESS);

	const ktx_uint8_t *const ktxTextureData = ktxTexture_GetData(ktxTexture);
	const ktx_size_t         ktxTextureSize = ktxTexture_GetSize(ktxTexture);
	//========================================================================================================================

	const VkExtent3D temp_image_extent{.width = ktxTexture->baseWidth, .height = ktxTexture->baseHeight, .depth = ktxTexture->baseDepth};

	constexpr bool force_linear_tiling = false;
	VkBool32       ktx_use_staging     = true;

	if (force_linear_tiling)
	{
		// Don't use linear tiling if format is not supported for (linear) shader sampling
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

		//创建完staging buffer以后，把cpu-side中的texture中的数据拷贝进staging buffer
		//TODO: whether to use memReqs.size or just ktxTexureSize 用ktxTexturesSize是对的。
		staging_buffer->CopyFrom((void const *) ktxTextureData, ktxTextureSize);
		//========================================================================================================================

		//TODO:这里没有考虑3维材质的问题,，
		//Setup buffer copy regions for each mip level

		std::vector<VkBufferImageCopy> buffer_copy_regions;

		for (uint32_t face = 0; face < ktxTexture->numFaces; face++)
		{
			for (uint32_t level = 0; level < ktxTexture->numLevels; level++)
			{
				// Calculate offset into staging buffer for the current mip level
				ktx_size_t     offset;
				KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, level, 0, face, &offset);
				assert(ret == KTX_SUCCESS);
				// Setup a buffer image copy structure for the current mip level
				VkBufferImageCopy bufferCopyRegion{};
				bufferCopyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel       = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
				bufferCopyRegion.imageSubresource.layerCount     = 1;
				bufferCopyRegion.imageExtent.width               = (ktxTexture->baseWidth) >> level;        //除法
				bufferCopyRegion.imageExtent.height              = (ktxTexture->baseHeight) >> level;
				bufferCopyRegion.imageExtent.depth               = 1;
				bufferCopyRegion.bufferOffset                    = offset;
				buffer_copy_regions.push_back(bufferCopyRegion);
			}
		}

		VkImageCreateFlags tex_img_CI_flags = Vk::ImgCINillFlag;

		if (ktxTexture->numFaces > 1)
		{
			tex_img_CI_flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}

		TextureImgPP para_pack(tex_name, format_of_image, temp_image_extent, tex_img_CI_flags);

		para_pack.default_image_CI.mipLevels   = ktxTexture->numLevels;
		para_pack.default_image_CI.arrayLayers = ktxTexture->numFaces;

		texture_image = std::dynamic_pointer_cast<VkGeneralPurposeImage>(img_factory.ProduceImage(para_pack));

		texture_image->CopyBufferToImage(staging_buffer->GetGPUBuffer(), buffer_copy_regions, VkDeviceManager::CommandPoolType::transfor_command_pool);

		VkImageSubresourceRange subresource_range;
		subresource_range.baseMipLevel   = 0;
		subresource_range.levelCount     = ktxTexture->numLevels;
		subresource_range.baseArrayLayer = 0;
		subresource_range.layerCount     = ktxTexture->numFaces;

		texture_image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image_layout_, VkDeviceManager::CommandPoolType::graphics_command_pool, subresource_range);
		//========================================================================================================================
		info_from_file.numArrayLayers = ktxTexture->numFaces;
		info_from_file.numLevels      = ktxTexture->numLevels;
	}

	else
	{
		//TODO: handle situation when we can't use staging buffer
	}

	ktxTexture_Destroy(ktxTexture);
	//========================================================================================================================

	return texture_image;
}
