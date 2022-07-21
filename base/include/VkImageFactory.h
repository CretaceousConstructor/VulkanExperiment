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

	[[nodiscard]] std::shared_ptr<VkImageBase> ProduceImage(const ImagePP &para_pack) const;
	//VkBufferBundle is copyable without risks of memory leak

	//[[nodiscard]] VkImageBundle ProduceImageBundle(const ImagePP &para_pack, size_t bundle_size) const;
	[[nodiscard]] std::shared_ptr<VkImageBundle> ProduceImageBundlePtr(const ImagePP &para_pack, size_t bundle_size) const;


	//[[nodiscard]] VkImageBundle ProduceImageBundle(const SwapchainImgPP &para_pack) const;
	[[nodiscard]] std::shared_ptr<VkImageBundle> ProduceImageBundlePtr(const SwapchainImgPP &para_pack) const;

  private:
	[[nodiscard]] VkImage        BuildImage(ImagePP &para_pack) const;
	[[nodiscard]] VkDeviceMemory CreateAndBindMemory(ImagePP &para_pack, VkImage temp_image) const;
	[[nodiscard]] VkImageView    BuildImageView(ImagePP &para_pack, VkImage temp_image) const;
	static void                  TransitionImageLayout(const ImagePP &para_pack, const std::shared_ptr<VkImageBase>& result);

  private:
  protected:
	VkGraphicsComponent &     gfx;
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkCommandManager &  command_manager;
	const VkWindows &         window;
};
