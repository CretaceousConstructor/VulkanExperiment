
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//


#include "VkTexture.h"



VkTexture::VkTexture(VkGraphicsComponent &gfx_, std::string image_path, std::shared_ptr<VkGeneralPurposeImage> image_, VkSampler texture_sampler_,VkImageLayout imageLayout_) :
    gfx(gfx_),
	device_manager(gfx.DeviceMan()),
    tex_name(std::move(image_path)),
    texture_image(image_),
    texture_sampler(texture_sampler_),
    image_layout(imageLayout_)
{


}

VkTexture::~VkTexture()
{
	vkDestroySampler(gfx.DeviceMan().GetLogicalDevice(), texture_sampler, nullptr);
}


VkWriteDescriptorSet VkTexture::GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement)
{
	imageInfo.imageLayout = image_layout;
	imageInfo.imageView   = GetTextureImageView();
	imageInfo.sampler     = GetTextureSampler();

	VkWriteDescriptorSet temp_writeDescriptorSet{};
	temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_writeDescriptorSet.dstBinding      = dstbinding;
	temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	temp_writeDescriptorSet.pImageInfo      = &imageInfo;
	temp_writeDescriptorSet.descriptorCount = 1;
	temp_writeDescriptorSet.dstArrayElement = dstArrayElement;

	return temp_writeDescriptorSet;
}

VkImage VkTexture::GetTextureImage() const
{
	return texture_image->GetImage();
}

VkImageView VkTexture::GetTextureImageView() const
{
	return texture_image->GetImageView();
}

VkSampler VkTexture::GetTextureSampler() const
{
	return texture_sampler;
}

VkImageLayout VkTexture::GetImageLayout() const
{
	return image_layout;
}



//VkTexture::VkTexture(VkGraphicsComponent &_gfx, const std::string &image_path, VkImageLayout para_imageLayout) :
//    gfx(_gfx),
//    device_manager(gfx.DeviceMan()),
//    window(gfx.Window())
//{
//	if (true)
//	{
//		InitKTXTexture(image_path, format_of_texture, para_imageLayout);
//	}
//	else
//	{
//		InitTexture(image_path, format_of_texture, para_imageLayout);
//	}
//	InitSampler();
//}

/*
void VkTexture::InitTexture(const std::string& image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout)
{






	tex_name  = image_path.substr(image_path.find_last_of("\\/") + 1, image_path.length());

	// VK_FORMAT_B8G8R8A8_SRGB
	// VK_FORMAT_R8G8B8A8_UNORM;
	int texWidth, texHeight, texChannels;
	stbi_uc *pixels = stbi_load(image_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	////交换BR的顺序，使之成为RGB形式。
	//if (format_of_texture == VK_FORMAT_B8G8R8A8_SRGB) {
	//	for (int i = 0; i < texHeight; i++) {
	//		for (int j = 0; j < texWidth * 4; j += 4) {
	//			auto temp = pixels[i * texHeight * 4 + j];
	//			pixels[i * texHeight * 4 + j] = pixels[i * texHeight * 4 + j + 2];
	//			pixels[i * texHeight * 4 + j + 2] = temp;

	//		}
	//	}
	//}
	const VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}
	//TODO: buffer创建简化
	VkBuffer       stagingBuffer;        //host visible memory
	VkDeviceMemory stagingBufferMemory;


	gfx.DeviceMan().CreateBufferAndBindToMemo(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());

	void *data;
	vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, imageSize, (VkMemoryMapFlags) 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);
	stbi_image_free(pixels);
	//TODO: buffer创建简化


	VkExtent3D image_extent;
	image_extent.width  = texWidth;
	image_extent.height = texHeight;
	image_extent.depth  = 1;

	VkDeviceManager::QueueFamilyIndices queue_family_index = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

	texture_image =
	    std::make_unique<VkGeneralPurposeImage>(device_manager, VK_IMAGE_TYPE_2D, format_of_texture, image_extent, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);



	texture_image->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);

	texture_image->CopyBufferToImage(stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), command_manager.transfer_command_pool, device_manager.GetTransferQueue());

	texture_image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);

	// Clean up staging resources
	vkDestroyBuffer(device_manager.GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, nullptr);


}

void VkTexture::InitTexture(const void *buffer, VkDeviceSize bufferSize, uint32_t texWidth, uint32_t texHeight,  VkFormat format_of_underlying_image, VkImageLayout para_imageLayout)
{
	//imageLayout    = para_imageLayout;

	if (!buffer)
	{
		throw std::runtime_error("buffer ptr is null!");
	}

	VkBuffer       stagingBuffer;        //host visible memory
	VkDeviceMemory stagingBufferMemory;
	device_manager.CreateBufferAndBindToMemo(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE,window.GetSurface());

	void *data;
	vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, (VkMemoryMapFlags) 0, &data);
	memcpy(data, buffer, static_cast<size_t>(bufferSize));
	vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);

	VkExtent3D image_extent;
	image_extent.width  = texWidth;
	image_extent.height = texHeight;
	image_extent.depth  = 1;

	VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager.FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

	texture_image=std::make_unique<VkGeneralPurposeImage>( device_manager,VK_IMAGE_TYPE_2D, format_of_underlying_image, image_extent, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);



	texture_image->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, command_manager.graphics_command_pool,device_manager.GetGraphicsQueue(), queue_family_index);




	texture_image->CopyBufferToImage(stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), command_manager.transfer_command_pool, device_manager.GetTransferQueue()); 



	texture_image->TransitionImageLayout( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);

	vkDestroyBuffer(device_manager.GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, nullptr);


}

void VkTexture::InitKTXTexture(const std::string& image_path, VkFormat format_of_underlying_image, VkImageLayout para_imageLayout)
{


	const std::string filename = image_path;
	VkFormat    format   = format_of_underlying_image;


	ktxResult   result;
	ktxTexture *ktxTexture;
	result = ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
	assert(result == KTX_SUCCESS);

	VkExtent3D image_extent;
	image_extent.width  = ktxTexture->baseWidth;
	image_extent.height = ktxTexture->baseHeight;
	image_extent.depth  = ktxTexture->baseDepth;

	mip_levels                  = ktxTexture->numLevels;



	ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
	ktx_size_t   ktxTextureSize = ktxTexture_GetSize(ktxTexture);


	// 如果要强行使用linear tiling，则 如果形式参数format_of_texure支持VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT，就不使用staging buffer，否则使用staging buffer
	bool forceLinearTiling = false;

	if (forceLinearTiling)
	{
		// Don't use linear if format is not supported for (linear) shader sampling
		// Get device properties for the requested texture format
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(device_manager.GetPhysicalDevice(), format, &formatProperties);
		ktx_use_staging = !(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
	}

	if (ktx_use_staging)
	{
		// Copy data to an optimal tiled image
		// This loads the texture data into a host local buffer that is copied to the optimal tiled image on the device

		// Create a host-visible staging buffer that contains the raw image data
		// This buffer will be the data source for copying texture data to the optimal tiled image on the device

		VkBuffer       stagingBuffer;        //host visible memory
		VkDeviceMemory stagingBufferMemory;

		device_manager.CreateBufferAndBindToMemo(ktxTextureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());

		void *data;
		//TODO: whether to use memReqs.size or just ktxTexureSize
		//VK_CHECK_RESULT(vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **) &data));
		vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, ktxTextureSize, (VkMemoryMapFlags) 0, (void **) &data);
		memcpy(data, ktxTextureData, static_cast<size_t>(ktxTextureSize));
		vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);

		//创建完staging buffer以后，把texture中的数据拷贝进staging buffer

		// Setup buffer copy regions for each mip level
		std::vector<VkBufferImageCopy> bufferCopyRegions;

		for (uint32_t i = 0; i < mip_levels; i++)
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

		VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager.FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

		texture_image=std::make_unique<VkGeneralPurposeImage>(device_manager,VK_IMAGE_TYPE_2D, format, image_extent, mip_levels, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );



		texture_image->TransitionImageLayout( VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index, mip_levels);



		texture_image->CopyBufferToImage(stagingBuffer, bufferCopyRegions,command_manager.transfer_command_pool,  device_manager.GetTransferQueue() );

		texture_image->TransitionImageLayout( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index, mip_levels);

		vkFreeMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, nullptr);
		vkDestroyBuffer(device_manager.GetLogicalDevice(), stagingBuffer, nullptr);
	}

	else
	{
		//TODO: handle situation when we can't use staging buffer
	}

	ktxTexture_Destroy(ktxTexture);



}

void VkTexture::InitTextureView(VkFormat format, VkImageAspectFlags aspectFlags)
{
	//
	if (ktx_use_staging)
	{
		texture_image->InitImageView(format, aspectFlags, mip_levels);
	}
	else
	{
		texture_image->InitImageView(format, aspectFlags, 1);
	}
}

void VkTexture::InitSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy    = properties.limits.maxSamplerAnisotropy;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp     = VK_COMPARE_OP_ALWAYS;

	//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
	samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod     = 0.0f;
	samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;

	if (vkCreateSampler(device_manager.GetLogicalDevice(), &samplerInfo, nullptr, &texture_sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void VkTexture::InitSampler(const VkSamplerCreateInfo& sampler_CI)
{
	if (vkCreateSampler(device_manager.GetLogicalDevice(), &sampler_CI, nullptr, &texture_sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}*/

