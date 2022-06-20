#include "VkImageBase.h"

VkImageBase::VkImageBase(VkGraphicsComponent &_gfx   , const VkImage _image, const VkFormat _image_format,  const VkImageView _image_view,const VkFormat _image_view_format) :
    gfx(_gfx), device_manager(gfx.DeviceMan()),
	image(_image),
	image_format(_image_format),
    image_view(_image_view),
	image_view_format(_image_view_format)

{}




VkImage VkImageBase::GetImage()const
{
	return image;
}

VkFormat VkImageBase::GetImageFormat()const
{
	return image_format;
}

VkImageView VkImageBase::GetImageView()const
{
	return image_view;
}

VkFormat VkImageBase::GetImageViewFormat()const
{
	return image_view_format;
}
