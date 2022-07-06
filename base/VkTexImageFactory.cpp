#include "VkTexImageFactory.h"

//std::shared_ptr<VkGeneralPurposeImage> VkTexImageFactory::ProduceImage(const ParameterPack &para_pack)const
//{
//	const auto image = BuildImage(para_pack);
//	const auto image_mem  = CreateAndBindMemory(para_pack,image);
//	const auto image_view =  BuildImageView(para_pack,image);
//
//	auto result = std::make_shared<VkGeneralPurposeImage>(gfx, image, image_mem, image_view, para_pack.format_of_image, para_pack.format_of_image);
//
//	//ownership is implicitly acquired upon first use within a queue.
//	TransitionImageLayout(para_pack, result);
//
//	return result;
//
//
//}
//
//VkTexImageFactory::VkTexImageFactory(VkGraphicsComponent &_gfx):
//    gfx(_gfx), device_manager(gfx.DeviceMan())
//{
//
//}
//
//
//
//VkImage VkTexImageFactory::BuildImage(const ParameterPack &para_pack) const
//{
//	VkImage temp_image;
//	if (vkCreateImage(device_manager.GetLogicalDevice(), &para_pack.default_image_CI, nullptr, &temp_image) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create image!");
//	}
//	return temp_image;
//
//
//}
//
//VkDeviceMemory VkTexImageFactory::CreateAndBindMemory(const ParameterPack &para_pack, VkImage temp_image) const
//{
//
//
//	VkDeviceMemory       temp_image_mem;
//	VkMemoryRequirements memRequirements;
//	vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), temp_image, &memRequirements);
//
//	VkMemoryAllocateInfo allocInfo{};
//	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize  = memRequirements.size;
//	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, para_pack.default_image_mem_prop_flag, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index
//
//	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_image_mem) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate image memory!");
//	}
//
//	vkBindImageMemory(device_manager.GetLogicalDevice(), temp_image, temp_image_mem, 0);        //把两者联系起来
//
//
//	return temp_image_mem;	
//
//}
//
//VkImageView VkTexImageFactory::BuildImageView(const ParameterPack &para_pack, VkImage temp_image) const
//{
//
//
//	VkImageView           temp_image_view;
//	VkImageViewCreateInfo image_view_create_info{para_pack.default_image_view_CI};
//	image_view_create_info.image = temp_image;
//
//
//	if (vkCreateImageView(device_manager.GetLogicalDevice(), &image_view_create_info, nullptr, &temp_image_view) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create texture image view!");
//	}
//
//
//	return temp_image_view;
//
//}
//
//void VkTexImageFactory::TransitionImageLayout(const ParameterPack &para_pack, std::shared_ptr<VkGeneralPurposeImage> result) const
//{
//
//
//	//ownership is implicitly acquired upon first use within a queue.
//	result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_final_layout, VkDeviceManager::CommandPoolType::transfor_command_pool,para_pack.default_image_CI.mipLevels,para_pack.default_image_CI.arrayLayers);
//
//
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
