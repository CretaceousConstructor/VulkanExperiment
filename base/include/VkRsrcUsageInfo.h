#pragma once
#include "Vk.h"
#include "VkBufferBase.h"
#include "VkGraphicsComponent.h"
#include "VkTexture.h"
#include <memory>
#include <variant>
#include <vector>

class RsrcUsageInfoInPass
{
  public:
	RsrcUsageInfoInPass(Vk::RsrcInfoType info_t);
	[[nodiscard]] virtual Vk::SyncInfo GetSynInfo() const = 0;
	[[nodiscard]] Vk::RsrcInfoType     GetInfoType() const;

  public:
	virtual ~RsrcUsageInfoInPass() = default;

	RsrcUsageInfoInPass() = delete;

	RsrcUsageInfoInPass(const RsrcUsageInfoInPass &) = delete;
	RsrcUsageInfoInPass &operator=(const RsrcUsageInfoInPass &) = delete;

	RsrcUsageInfoInPass(RsrcUsageInfoInPass &&) = delete;
	RsrcUsageInfoInPass &operator=(RsrcUsageInfoInPass &&) = delete;

  private:
	Vk::RsrcInfoType info_type;
};

class VkTexUsageInfo
{
  public:
	VkDescriptorType   desc_type{};
	VkShaderStageFlags access_stages{};
	Vk::DescSetInfo    set_info{};

	std::shared_ptr<VkTexture> tex{};
};

class VkTexUsageInfoRG : public RsrcUsageInfoInPass
{
  public:
	VkTexUsageInfoRG(
	    Vk::RsrcInfoType info_y,
	    VkFormat         format_,
	    Vk::DescSetInfo  set_info_,

	    VkAccessFlags        access_mask_,
	    VkPipelineStageFlags stage_mask_,
	    VkImageLayout        layout_inpass_,
	    VkDescriptorType     type_) :
	    RsrcUsageInfoInPass(info_y),
	    format(format_),
	    set_info(set_info_),
	    access_mask(access_mask_),
	    stage_mask(stage_mask_),
	    layout_inpass(layout_inpass_),
	    tex_type(type_)

	{
	}

	~VkTexUsageInfoRG() = default;

	VkFormat format{};

	VkDescriptorType     tex_type{};
	VkPipelineStageFlags stage_mask{};
	VkAccessFlags        access_mask{};
	Vk::DescSetInfo      set_info{};

	VkImageLayout layout_inpass{};

	std::optional<VkSamplerCreateInfo>   sampler_CI{};
	std::optional<VkImageViewCreateInfo> img_view_CI{};

  public:
	[[nodiscard]] std::optional<VkSamplerCreateInfo> GetSamplerCI() const
	{
		return sampler_CI;
	}

	[[nodiscard]] std::optional<VkImageViewCreateInfo> GetImgViewCI() const
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

class VkUniBufUsageInfo
{
  public:
	VkDescriptorType   desc_type{};
	VkShaderStageFlags access_stages{};
	Vk::DescSetInfo    set_info{};

	std::shared_ptr<VkBufferBase> buf{};
};

class VkBufUsageInfoRG final : public RsrcUsageInfoInPass
{
  public:
	VkBufUsageInfoRG(
	    Vk::RsrcInfoType info_t,
	    Vk::DescSetInfo  slot_info_,
	    VkDescriptorType buf_type_,

	    VkPipelineStageFlags stage_mask_) :
	    RsrcUsageInfoInPass(info_t),
	    set_info(slot_info_),
	    buf_type(buf_type_),
	    stage_mask(stage_mask_)
	{
	}

	~VkBufUsageInfoRG() = default;

	VkDescriptorType     buf_type{};
	VkPipelineStageFlags stage_mask{};
	Vk::DescSetInfo      set_info{};

	[[nodiscard]] Vk::SyncInfo GetSynInfo() const override
	{
		return {};
	}
};

class VkAttachmentInfo
{
  public:
	enum struct Type
	{
		Unknown,
		ColorAttachment,
		DepthAttachment,
		DepthStencilAttachment,
		//StencilAttachment, // reserved for future use
		ResolveOpTargetAttachment,

		NotUsedWithinPass

	};

	struct DynamicRenderingAttachment
	{
	  public:
		DynamicRenderingAttachment() = default;
		DynamicRenderingAttachment(
		    Type     attach_type_,
		    VkFormat format_,
		    uint32_t index_);
		Type     attach_type;
		VkFormat format;

		//index ONLY use to sort DynamicRenderingAttachmentFormats, no any other usages.
		uint32_t index;
	};

	class WithinPassRG : public RsrcUsageInfoInPass
	{
	  public:
	  public:
		WithinPassRG(
		    Vk::RsrcInfoType                                       info_y,
		    VkFormat                                               format_,
		    std::variant<Vk::AttachmentIndexType, Vk::DescSetInfo> slot_info_,

		    VkAccessFlags        access_mask_,
		    VkPipelineStageFlags stage_mask_,
		    VkImageLayout        layout_inpass_,
		    Type                 type_,
		    VkClearValue         clear_value_,

		    VkResolveModeFlagBits resolve_mode_         = VK_RESOLVE_MODE_NONE,
		    VkImageLayout         resolve_image_layout_ = VK_IMAGE_LAYOUT_UNDEFINED,
		    //VkImageView              resolveImageView{};

		    VkAttachmentLoadOp  load_op_  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		    VkAttachmentStoreOp store_op_ = VK_ATTACHMENT_STORE_OP_DONT_CARE) :
		    RsrcUsageInfoInPass(info_y),
		    format(format_),
		    slot_info(slot_info_),
		    access_mask(access_mask_),
		    stage_mask(stage_mask_),
		    layout_inpass(layout_inpass_),
		    type(type_),
		    clear_value(clear_value_),
		    resolve_mode(resolve_mode_),
		    resolve_image_layout(resolve_image_layout_),
		    load_op(load_op_),
		    store_op(store_op_)

		{
		}

		~WithinPassRG() override = default;

		VkFormat format{};
		//uint32_t attachment_index{};

		std::variant<Vk::AttachmentIndexType, Vk::DescSetInfo> slot_info;
		//VkAttachmentLoadOp  loadOp{};
		//VkAttachmentStoreOp storeOp{};

		VkAccessFlags        access_mask{};
		VkPipelineStageFlags stage_mask{};
		VkImageLayout        layout_inpass{};
		Type                 type{Type::Unknown};
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

	struct WithinPass
	{
		WithinPass(
		    VkFormat               format_,
		    uint32_t               attachment_index_,
		    VkAttachmentLoadOp     loadOp_,
		    VkAttachmentStoreOp    storeOp_,
		    VkImageLayout          layout_prepass_,
		    VkImageLayout          layout_inpass_,
		    VkImageLayout          layout_afterpass_,
		    VkAttachmentInfo::Type type_,
		    VkClearValue           clear_value_,

		    VkResolveModeFlagBits resolveMode_ = VK_RESOLVE_MODE_NONE,
		    //VkImageView              resolveImageView{};
		    VkImageLayout resolveImageLayout_ = VK_IMAGE_LAYOUT_UNDEFINED) :

		    format(format_),
		    attachment_index(attachment_index_),
		    loadOp(loadOp_),
		    storeOp(storeOp_),
		    layout_prepass(layout_prepass_),
		    layout_inpass(layout_inpass_),
		    layout_afterpass(layout_afterpass_),
		    type(type_),
		    clear_value(clear_value_),
		    resolveMode(resolveMode_),
		    //VkImageView              resolveImageView{};
		    resolveImageLayout(resolveImageLayout_)

		{
		}

		WithinPass(const WithinPassRG &rhs) :
		    format(rhs.format),
		    attachment_index(std::get<Vk::AttachmentIndexType>(rhs.slot_info)),
		    loadOp(rhs.load_op),
		    storeOp(rhs.store_op),
		    layout_prepass(rhs.layout_inpass),
		    layout_inpass(rhs.layout_inpass),
		    layout_afterpass(rhs.layout_inpass),
		    type(rhs.type),
		    clear_value(rhs.clear_value),
		    resolveMode(rhs.resolve_mode),
		    //VkImageView              resolveImageView{};
		    resolveImageLayout(rhs.resolve_image_layout)
		{
		}

		VkFormat               format{};
		uint32_t               attachment_index{};
		VkAttachmentLoadOp     loadOp{};
		VkAttachmentStoreOp    storeOp{};
		VkImageLayout          layout_prepass{};
		VkImageLayout          layout_inpass{};
		VkImageLayout          layout_afterpass{};
		VkAttachmentInfo::Type type{Type::Unknown};
		VkClearValue           clear_value{};

		VkResolveModeFlagBits resolveMode{VK_RESOLVE_MODE_NONE};
		//VkImageView              resolveImageView{};
		VkImageLayout resolveImageLayout{};
	};


	void SetResolveTargetTexImgView(const VkAttachmentInfo& resolve_target)
	{
		resolve_tex = resolve_target.tex;
		if (!info_in_pass.has_value())
		{
			throw std::runtime_error("no valid usage info!");
		}
		else
		{
			info_in_pass.value().resolveImageLayout = resolve_target.GetInfo().layout_inpass;
		}
	}



	class Bundle
	{
	  public:
		Bundle() = default;
		Bundle(std::vector<VkAttachmentInfo> info_array_, DynamicRenderingAttachment attachment_format_);
		[[nodiscard]] DynamicRenderingAttachment GetAttachmentFormatAndType() const;
		VkAttachmentInfo &                       operator[](size_t index);

	  private:
		std::vector<VkAttachmentInfo> info_array;
		DynamicRenderingAttachment    attachment_format{};
	};

  public:
	VkAttachmentInfo(const WithinPassRG &info, std::shared_ptr<VkTexture> tex_);
	VkAttachmentInfo(const WithinPassRG &info, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_);


	VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_);
	VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_);

	VkAttachmentInfo() = default;

	[[nodiscard]] const WithinPass &GetInfo() const;

	[[nodiscard]] DynamicRenderingAttachment GetAttachmentFormatAndType() const;
	[[nodiscard]] VkRenderingAttachmentInfo  GetRenderingAttachmentInfo();
	void                                     SetMemento(WithinPass meme_);
	void                                     SetTexture(std::shared_ptr<VkTexture> tex);
	[[nodiscard]] const VkTexture &          GetTex() const;
	[[nodiscard]] VkTexture &                GetTex();

  public:
	//static Bundle GetAttachmentInfos(const Memento &meme_);
	static Bundle GetAttachmentInfos(const WithinPass &meme_, const VkTexture::TexturePtrBundle &textures_);
	static Bundle GetAttachmentInfos(const WithinPass &meme_, const VkTexture::TexturePtrBundle &textures_, const VkTexture::TexturePtrBundle &resolve_textures_);




  private:

	VkRenderingAttachmentInfo  rendering_attachment_info{}; //provided by vulkan header, will be pass to VkBeginRendering funciton
	std::optional<WithinPass>  info_in_pass;
	std::shared_ptr<VkTexture> tex;
	std::shared_ptr<VkTexture> resolve_tex;
};



namespace AttachmentInfoGeneration
{
template <typename T>
inline void Process(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    std::optional<size_t>                   img_index,
    T &);

template <>
inline void Process(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    std::optional<size_t>                   img_index,
    VkAttachmentInfo &                      attach)
{
	const auto attach_format_and_type = attach.GetAttachmentFormatAndType();
	if (VkAttachmentInfo::Type::ColorAttachment == attach_format_and_type.attach_type)
	{
		color_attachment_infos.push_back(attach.GetRenderingAttachmentInfo());
	}
	if (VkAttachmentInfo::Type::DepthAttachment == attach_format_and_type.attach_type || VkAttachmentInfo::Type::DepthStencilAttachment == attach_format_and_type.attach_type)
	{
		depth_attachment_info = attach.GetRenderingAttachmentInfo();
	}
}

template <>
inline void Process(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    std::optional<size_t>                   img_index,
    VkAttachmentInfo::Bundle &              attachment_bundle)
{
	const auto attach_format_and_type = attachment_bundle.GetAttachmentFormatAndType();

	if (VkAttachmentInfo::Type::ColorAttachment == attach_format_and_type.attach_type)
	{
		color_attachment_infos.push_back(attachment_bundle[img_index.value()].GetRenderingAttachmentInfo());
	}

	if (VkAttachmentInfo::Type::DepthAttachment == attach_format_and_type.attach_type || VkAttachmentInfo::Type::DepthStencilAttachment == attach_format_and_type.attach_type)
	{
		depth_attachment_info = attachment_bundle[img_index.value()].GetRenderingAttachmentInfo();
	}
}

//recursion base
template <typename T>
void GenerateRenderingAttachInfo(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    size_t                                  img_index,
    T &&                                    first)
{
	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, img_index, std::forward<T>(first));
	return;
}

//function call
template <typename First, typename... Rest>
void GenerateRenderingAttachInfo(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    size_t                                  img_index,
    First &&                                first,
    Rest &&...rest)
{
	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, img_index, std::forward<First>(first));
	return GenerateRenderingAttachInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, img_index, std::forward<Rest>(rest)...);
}

//recursion base
template <typename T>
void GenerateRenderingAttachInfo(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    T &&                                    first)
{
	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, std::nullopt, std::forward<T>(first));
	return;
}
//function call
template <typename First, typename... Rest>
void GenerateRenderingAttachInfo(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    First &&                                first,
    Rest &&...rest)
{
	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, std::nullopt, std::forward<First>(first));
	return AddRenderingAttachmentInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, std::forward<Rest>(rest)...);
}

inline void GenerateRenderingAttachInfo(
    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
    VkRenderingAttachmentInfo &             depth_attachment_info,
    VkRenderingAttachmentInfo &             stensil_attachment_info,
    std::vector<VkAttachmentInfo> &         attachment_infos)
{
	for (auto &attach_info : attachment_infos)
	{
		Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, std::nullopt, attach_info);
	}
}

}        // namespace AttachmentInfoGeneration
