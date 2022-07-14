#pragma once
#include "VkBufferBundle.h"
#include "VkHeader.h"

#include <thread>
#include <vector>

namespace Vk
{

	enum class Type
	{
		Sampler



	};


	//template const expression in replace of magic number in code.
	template <uint32_t T>
	constexpr uint32_t Binding{T};

	template <uint32_t T>
	constexpr uint32_t Offset{T};

	template <uint32_t T>
	constexpr uint32_t AttachmentRef{T};

	//template variable in replace of magic number in code.

	template <Vk::Type Sampler>
	size_t UUID_safe{0};

	template <Vk::Type Sampler>
	size_t UUID_unsafe{0};




//....
VkAttachmentReference           GetAttachmentReference(uint32_t attachment, VkImageLayout layout);
VkDescriptorPoolSize            GetOneDescriptorPoolSizeDescription(VkDescriptorType type, uint32_t descriptor_count);
VkDescriptorPoolCreateInfo      GetDescriptorPoolCI(const std::vector<VkDescriptorPoolSize> &pool_sizes, uint32_t max_sets, const void *pNext = nullptr, VkDescriptorPoolCreateFlags flags = 0);
VkDescriptorSetLayoutBinding    GetDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t descriptor_count = 1, const VkSampler *pImmutableSamplers = nullptr);
VkDescriptorSetLayoutCreateInfo GetDescriptorSetLayoutCI(const std::vector<VkDescriptorSetLayoutBinding> &bindings, const void *pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);


VkPipelineLayout                GetPipelineLayout(const VkDevice &device, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges, const void *pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
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

}        // namespace Vk
