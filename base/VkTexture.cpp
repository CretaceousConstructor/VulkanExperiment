
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "VkTexture.h"

VkTexture::VkTexture(VkGraphicsComponent &gfx_, std::string image_path, std::shared_ptr<VkGeneralPurposeImage> image_, VkSampler texture_sampler_, VkImageLayout imageLayout_) :
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

VkWriteDescriptorSet VkTexture::GetWriteDescriptorSetInfo( VkDescriptorSet set,uint32_t dstbinding, uint32_t dstArrayElement)
{
	imageInfo.imageLayout = image_layout;
	imageInfo.imageView   = GetTextureImageView();
	imageInfo.sampler     = GetTextureSampler();

	VkWriteDescriptorSet temp_writeDescriptorSet{};
	temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_writeDescriptorSet.dstBinding      = dstbinding;
	temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	temp_writeDescriptorSet.dstSet          = set;
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
