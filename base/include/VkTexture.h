#pragma once

#include "VkGeneralPurposeImage.h"
#include "VkGraphicsComponent.h"
#include <ktx.h>
#include <ktxvulkan.h>
#include <memory>
#include <string>

class VkTexture
{


  public:
	using TexturePtrBundle = std::vector<std::shared_ptr<VkTexture>>;
	using TextureBundle    = std::vector<VkTexture>;
	using TexturePtr       = std::shared_ptr<VkTexture>;

  public:
	struct Descriptor
	{
		std::optional<TextureImgPP>          tex_img_PP;
		std::optional<VkImageViewCreateInfo> img_view_CI;
		std::optional<VkSamplerCreateInfo>   sample_CI;
		uint32_t                             bundle_size{};   //maybe not needed
	};


	class WithinPassRG : public Vk::RsrcUsageInfoInPass
	{
	  public:
		WithinPassRG(
		    Vk::RsrcInfoType info_y,
		    VkFormat         format_,
		    //uint32_t         attachment_index_,
		    //VkAttachmentLoadOp  loadOp{};
		    //VkAttachmentStoreOp storeOp{};
			
			
		    VkAccessFlags        access_mask_,
		    VkPipelineStageFlags stage_mask_,
		    VkImageLayout        layout_inpass_
		    //VkClearValue         clear_value_,

		    //VkResolveModeFlagBits resolve_mode_         = VK_RESOLVE_MODE_NONE,
		    //VkImageLayout         resolve_image_layout_ = VK_IMAGE_LAYOUT_UNDEFINED,
		    //VkImageView              resolveImageView{};

		    //VkAttachmentLoadOp  load_op_   = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		    //VkAttachmentStoreOp store_op_ = VK_ATTACHMENT_STORE_OP_DONT_CARE
		) :
		    RsrcUsageInfoInPass(info_y),
		    format(format_),
		    //attachment_index(attachment_index_),
		    access_mask(access_mask_),
		    stage_mask(stage_mask_),
		    layout_inpass(layout_inpass_),
		    clear_value(clear_value_),
		    resolve_mode(resolve_mode_),
		    resolve_image_layout(resolve_image_layout_),
			load_op(load_op_),
		    store_op(store_op_)

		{
		}

		~WithinPassRG() = default;

		VkFormat format{};
		uint32_t attachment_index{};
		//VkAttachmentLoadOp  loadOp{};
		//VkAttachmentStoreOp storeOp{};

		VkAccessFlags        access_mask{};
		VkPipelineStageFlags stage_mask{};
		VkImageLayout        layout_inpass{};
		VkClearValue         clear_value{};

		VkResolveModeFlagBits resolve_mode{};
		VkImageLayout         resolve_image_layout{};
		//VkImageView              resolveImageView{};
		VkAttachmentLoadOp  load_op;
		VkAttachmentStoreOp store_op;

		std::optional<VkSamplerCreateInfo>   sampler_CI;
		std::optional<VkImageViewCreateInfo> img_view_CI;

		std::optional<VkSamplerCreateInfo> GetSamplerCI() const
		{
			return sampler_CI;
		}

		std::optional<VkImageViewCreateInfo> GetImgViewCI() const
		{
			return img_view_CI;
		}

		[[nodiscard]] Vk::SyncInfo GetSynInfo() const override
		{
			return {
			    .access_mask   = access_mask,
			    .stage_mask    = stage_mask,
			    .layout_inpass = layout_inpass};
		}
	};































  public:
	friend class VkTextureFactory;
	VkTexture(VkGraphicsComponent &gfx_, std::string image_path, std::shared_ptr<VkImageBase> image_, VkImageLayout imageLayout_);
	VkTexture()  = delete;
	~VkTexture() = default;

	VkTexture(const VkTexture &) = delete;
	VkTexture &operator=(const VkTexture &) = delete;

	VkTexture(VkTexture &&) = delete;
	VkTexture &operator=(VkTexture &&) = delete;

  public:
	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement = 0);
	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement = 0);

  public:
	[[nodiscard]] std::shared_ptr<VkImageBase> GetTextureImage() const;
	[[nodiscard]] VkImage                      GetTextureRawImage() const;
	[[nodiscard]] VkImageView                  GetTextureImageView() const;
	[[nodiscard]] VkSampler                    GetTextureSampler() const;
	[[nodiscard]] VkImageLayout                GetImageLayout() const;

  public:
	void InsertImageMemoryBarrier(VkCommandBuffer cmd_buffer, const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier);

	//这个函数目前需要修改，修改的目的是让用户去解决具体该怎么写image barrier，而不是通过一堆if else去假设。
	void InsertImageMemoryBarrier(const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier, const VkDeviceManager::CommandPoolType command_type);

  private:
	std::optional<VkSamplerCreateInfo>   sampler_CI;
	std::optional<VkImageViewCreateInfo> img_view_CI;

  private:
	void SetCISamplerAndImgView(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_);

  private:
	void SetImgViewCI(VkImageViewCreateInfo image_view_);
	void SetSamplerCI(VkSamplerCreateInfo texture_sampler_);

  private:
	std::string                  tex_name;
	std::shared_ptr<VkImageBase> tex_image;

	VkImageView tex_image_view{};
	VkSampler   tex_sampler{};

	VkDescriptorImageInfo image_info{};
	VkImageLayout         current_image_layout{};
	VkBool32              ktx_use_staging = true;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};
