#pragma once
#include "VkGraphicsComponent.h"
#include "VkImageBase.h"
#include "VkImageBundle.h"
#include "ImageParameterPack.h"
#include "VkSwapchainImage.h"
#include "VkGeneralPurposeImage.h"


class VkImageFactory
{
  public:
	VkImageFactory(VkGraphicsComponent &_gfx);
	~VkImageFactory() = default;

	VkImageFactory()                       = delete;
	VkImageFactory(const VkImageFactory &) = delete;
	VkImageFactory &operator=(const VkImageFactory &) = delete;

	VkImageFactory(VkImageFactory &&) = delete;
	VkImageFactory &operator=(VkImageFactory &&) = delete;

	[[nodiscard]] std::shared_ptr<VkImageBase> ProduceImage(const ImgParameterPack &para_pack) const;
	//VkBufferBundle is copyable without risks of memory leak

	//[[nodiscard]] VkImageBundle ProduceImageBundle(const ImgParameterPack &para_pack, size_t bundle_size) const;
	[[nodiscard]] std::shared_ptr<VkImageBundle> ProduceImageBundlePtr(const ImgParameterPack &para_pack, size_t bundle_size) const;


	//[[nodiscard]] VkImageBundle ProduceImageBundle(const SwapImgParameterPack &para_pack) const;
	[[nodiscard]] std::shared_ptr<VkImageBundle> ProduceImageBundlePtr(const SwapImgParameterPack &para_pack) const;

  private:
	[[nodiscard]] VkImage        BuildImage(ImgParameterPack &para_pack) const;
	[[nodiscard]] VkDeviceMemory CreateAndBindMemory(ImgParameterPack &para_pack, VkImage temp_image) const;
	[[nodiscard]] VkImageView    BuildImageView(ImgParameterPack &para_pack, VkImage temp_image) const;
	static void                  TransitionImageLayout(const ImgParameterPack &para_pack, const std::shared_ptr<VkImageBase>& result);

  private:
  protected:
	VkGraphicsComponent &     gfx;
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkCommandManager &  command_manager;
	const VkWindows &         window;
};
