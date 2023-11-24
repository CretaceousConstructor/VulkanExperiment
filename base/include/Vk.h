#pragma once
#include "VkHeader.h"
#include <optional>
#include <thread>
#include <vector>

namespace Vk
{



constexpr VkImageCreateFlags ImgCINillFlag{0};
constexpr int                MAX_FRAMES_IN_FLIGHT{2};
constexpr uint32_t           SWAP_IMG_COUNT{3};
constexpr VkFlags            NoneFlag{0};

constexpr VkFormat             required_depth_formats[]{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
constexpr VkFormatFeatureFlags required_depth_format_feature{VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT};



using AttachmentIndexType = uint32_t;

template <uint32_t T>
constexpr uint32_t SetIndex{T};

template <uint32_t T>
constexpr uint32_t Binding{T};

template <uint32_t T>
constexpr uint32_t BindingArrayElement{T};

template <uint32_t T>
constexpr uint32_t Offset{T};

template <uint32_t T>
constexpr uint32_t AttachmentRef{T};

template <uint32_t T>
constexpr uint32_t AttachmentIndex{T};

constexpr uint32_t InvalidAttachIndex{AttachmentIndex<0xffffffff>};

template <uint32_t T>
constexpr uint32_t BindingCount{T};

template <uint32_t T>
constexpr uint32_t SetCount{T};

template <uint32_t T>
constexpr uint32_t DescriptorCount{T};

template <uint32_t T>
constexpr uint32_t BundleSize{T};

struct SyncInfo
{
	VkAccessFlags        access_mask{};
	VkPipelineStageFlags stage_mask{};
	VkImageLayout        layout_inpass{};
};



enum class RsrcInfoType
{
	Unknown,
	Attachment,
	Buffer,
	Texture,
	Reference,
	Model
};
enum class AccessType
{
	Unknown,
	Write,
	Read,
	ReadWrite,
};


struct DescSetInfo
{
	uint32_t set{0};
	uint32_t binding{0};
	uint32_t array_elemnt{0};
};

struct EnumClassHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};


struct ResourceDimensions
{
	VkFormat format = VK_FORMAT_UNDEFINED;
	//BufferInfo buffer_info;
	unsigned width   = 0;
	unsigned height  = 0;
	unsigned depth   = 1;
	unsigned layers  = 1;
	unsigned levels  = 1;
	unsigned samples = 1;
	//AttachmentInfoFlags flags = ATTACHMENT_INFO_PERSISTENT_BIT;
	VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	//RenderGraphQueueFlags queues = 0;
	VkImageUsageFlags image_usage = 0;
	bool              operator==(const ResourceDimensions &other) const
	{
		return format == other.format &&
		       width == other.width &&
		       height == other.height &&
		       depth == other.depth &&
		       layers == other.layers &&
		       levels == other.levels &&
		       //buffer_info == other.buffer_info &&
		       //flags == other.flags &&
		       transform == other.transform;
		// image_usage is deliberately not part of this test.
		// queues is deliberately not part of this test.
	}
};



enum ModelLoadingOption
{
	LoadingImgByOurselves = 0x1,
	BindlessRenderingMode = 0x2,
	None                  = 0x4,
};

enum class Type
{
	Sampler
};

//template variables in replace of magic numbers in code.

template <Vk::Type Sampler>
size_t UUID_safe{0};

template <Vk::Type Sampler>
size_t UUID_unsafe{0};

[[nodiscard]] std::string GetFileExtensionName(const std::string &filename);
[[nodiscard]] bool        HasStencilComponent(VkFormat format);

//....
VkAttachmentReference GetAttachmentReference(uint32_t attachment, VkImageLayout layout);
VkDescriptorPoolSize  GetOneDescriptorPoolSizeDescription(VkDescriptorType type, uint32_t descriptor_count);
//make those functions accept universal ref, to prevent rvalue being passed in.
VkDescriptorPoolCreateInfo GetDescriptorPoolCI(const std::vector<VkDescriptorPoolSize> &pool_sizes, uint32_t max_sets, VkDescriptorPoolCreateFlags flags = 0, const void *pNext = nullptr);
//make those functions accept universal ref, to prevent rvalue being passed in.
VkDescriptorSetLayoutBinding GetDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t descriptor_count = 1, const VkSampler *pImmutableSamplers = nullptr);
//make those functions accept universal ref, to prevent rvalue being passed in.
VkDescriptorSetLayoutCreateInfo GetDescriptorSetLayoutCI(const std::vector<VkDescriptorSetLayoutBinding> &bindings, VkDescriptorSetLayoutCreateFlags flags = 0, const void *pNext = nullptr);

//make those functions accept universal ref, to prevent rvalue being passed in.
VkPipelineLayout GetPipelineLayout(const VkDevice &device, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges, const void *pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
//....

//********************************************************************
VkPushConstantRange GetPushConstantRange(VkShaderStageFlags stage_flags, uint32_t size, uint32_t offset);

VkVertexInputAttributeDescription GetVertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);
VkVertexInputBindingDescription   GetVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

VkPipelineVertexInputStateCreateInfo GetVertexInputStateCI(const std::vector<VkVertexInputBindingDescription> &vertex_input_binding_descriptions, const std::vector<VkVertexInputAttributeDescription> &vertex_input_attribute_description);

VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyStateCI(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitiveRestartEnable);

VkPipelineDynamicStateCreateInfo GetDynamicStateCI(const std::vector<VkDynamicState> &dynamic_states, VkPipelineDynamicStateCreateFlags flags);

//********************************************************************

VkDescriptorSetAllocateInfo GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const VkDescriptorSetLayout &SetLayout);
VkDescriptorSetAllocateInfo GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const std::vector<VkDescriptorSetLayout> &SetLayouts);
//********************************************************************

size_t GetSamplerUUID_unsafe();

//********************************************************************

}        // namespace Vk

namespace Sync
{
struct VkImageMemoryBarrierEnhanced
{
	VkAccessFlags        srcAccessMask{VK_ACCESS_NONE};
	VkPipelineStageFlags srcStageMask{VK_PIPELINE_STAGE_NONE};
	VkImageLayout        oldLayout{VK_IMAGE_LAYOUT_UNDEFINED};

	VkAccessFlags        dstAccessMask{VK_ACCESS_NONE};
	VkPipelineStageFlags dstStageMask{VK_PIPELINE_STAGE_NONE};
	VkImageLayout        newLayout{VK_IMAGE_LAYOUT_UNDEFINED};

	uint32_t srcQueueFamilyIndex{VK_QUEUE_FAMILY_IGNORED};
	uint32_t dstQueueFamilyIndex{VK_QUEUE_FAMILY_IGNORED};

	std::optional<VkImageSubresourceRange> subresource_range;
};

}        // namespace Sync
