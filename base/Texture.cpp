#include "Texture.h"
//#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include "stb_image.h"



void Texture::CleanUp()
{
	if (device_manager != nullptr) {

		vkDestroySampler(device_manager->GetLogicalDeviceRef(), texture_sampler, nullptr);

	}
	else {
		throw std::runtime_error("device_manager is a nullptr ");
	}
}

void Texture::InitTexture(std::string image_namge, VkDeviceManager* para_device_manager, VkWindows* window, VkCommandPool& command_pool, VkFormat format_of_texture)
{


	// VK_FORMAT_B8G8R8A8_SRGB 
	// VK_FORMAT_R8G8B8A8_UNORM;
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(image_namge.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (format_of_texture == VK_FORMAT_B8G8R8A8_SRGB) {
		for (int i = 0; i < texHeight; i++) {
			for (int j = 0; j < texWidth * 4; j += 4) {
				auto temp = pixels[i * texHeight * 4  + j];
				pixels[i * texHeight*4 + j] = pixels[i * texHeight*4 + j + 2];
				pixels[i * texHeight*4 + j + 2]  = temp;
				
			}
		}
	}

	device_manager = para_device_manager;
	VkDeviceSize imageSize = texWidth * texHeight * 4;

















	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer; //host visible memory
	VkDeviceMemory stagingBufferMemory;
	device_manager->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());

	void* data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, imageSize, (VkMemoryMapFlags)0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

	stbi_image_free(pixels);



	VkExtent3D image_extent;
	image_extent.width = texWidth;
	image_extent.height = texHeight;
	image_extent.depth = 1;






	texture_image.Init(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, image_extent, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);
	texture_image.TransitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, command_pool, para_device_manager->GetLogicalDeviceRef(), para_device_manager->GetGraphicsQueue());
	texture_image.CopyBufferToImage(stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), command_pool, para_device_manager->GetLogicalDeviceRef(), para_device_manager->GetGraphicsQueue());

	vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
	vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);






}

void Texture::InitTextureView(VkFormat format, VkImageAspectFlags aspectFlags)
{
	texture_image.InitImageView(format, aspectFlags);
}

void Texture::InitSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(device_manager->GetPhysicalDeviceRef(), &properties);
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;




	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;




	//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;


	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;




	if (vkCreateSampler(device_manager->GetLogicalDeviceRef(), &samplerInfo, nullptr, &texture_sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}



}

VkImage& Texture::GetTextureImage()
{

	return texture_image.GetImageRef();
}

VkImageView& Texture::GetTextureImageView()
{

	return texture_image.GetImageView();
}

VkSampler& Texture::GetTextureSampler()
{

	return texture_sampler;
}
