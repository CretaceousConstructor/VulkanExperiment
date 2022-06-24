#pragma once
#include "VkImageFactory.h"
#include "VkGeneralPurposeImage.h"


class VkDepthImageFactory : public VkImageFactory
{
public:
	VkDepthImageFactory(VkGraphicsComponent& _gfx);




protected:
	void BuildImage() override;
	void CreateAndBindMemory() override;
	void BuildImageView() override;
	void Assemble() override;
	void TransitionImageLayout() override;
private:
	void RestoreToDefaultState() override;



private:
	VkImageCreateInfo        default_image_CI{};
	VkMemoryPropertyFlagBits default_image_mem_prop_flag{};
	VkExtent3D               default_image_extent{};
	VkFormat                 default_image_format{};
	VkImageLayout            default_final_layout{};
private:
	VkImage temp_image{};
	VkDeviceMemory temp_image_mem{};
	VkImageView temp_image_view{};

};

