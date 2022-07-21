#include "Vk.h"

VkAttachmentReference Vk::GetAttachmentReference(uint32_t attachment, VkImageLayout layout)
{
	return {attachment, layout};
}

VkPipelineLayout Vk::GetPipelineLayout(const VkDevice &device, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges, const void *pNext, VkDescriptorSetLayoutCreateFlags flags)
{
	VkPipelineLayout result{};

	VkPipelineLayoutCreateInfo pipeline_layoutCI{};
	pipeline_layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layoutCI.pNext = pNext;
	pipeline_layoutCI.flags = flags;

	pipeline_layoutCI.setLayoutCount = uint32_t(set_layouts.size());
	pipeline_layoutCI.pSetLayouts    = set_layouts.data();

	//TODO: testing multiple push constants and how to access it
	// We will use push constants to push the local matrices of a primitive to the vertex shader
	pipeline_layoutCI.pushConstantRangeCount = uint32_t(push_constant_ranges.size());
	pipeline_layoutCI.pPushConstantRanges    = push_constant_ranges.data();

	VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipeline_layoutCI, nullptr, &result));

	return result;
}



VkPushConstantRange Vk::GetPushConstantRange(VkShaderStageFlags stage_flags, uint32_t size, uint32_t offset)
{
	return {stage_flags, offset, size};
}

VkVertexInputAttributeDescription Vk::GetVertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
{
	VkVertexInputAttributeDescription vInputAttribDescription{};
	vInputAttribDescription.location = location;
	vInputAttribDescription.binding  = binding;
	vInputAttribDescription.format   = format;
	vInputAttribDescription.offset   = offset;
	return vInputAttribDescription;
}

VkVertexInputBindingDescription Vk::GetVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
	VkVertexInputBindingDescription vInputBindDescription{};
	vInputBindDescription.binding   = binding;
	vInputBindDescription.stride    = stride;
	vInputBindDescription.inputRate = inputRate;
	return vInputBindDescription;
}

VkPipelineVertexInputStateCreateInfo Vk::GetVertexInputStateCI(const std::vector<VkVertexInputBindingDescription> &vertex_input_binding_descriptions, const std::vector<VkVertexInputAttributeDescription> &vertex_input_attribute_description)
{
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
	pipelineVertexInputStateCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(vertex_input_binding_descriptions.size());
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions      = vertex_input_binding_descriptions.data();
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attribute_description.size());
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions    = vertex_input_attribute_description.data();
	return pipelineVertexInputStateCreateInfo;
}

VkPipelineInputAssemblyStateCreateInfo Vk::GetInputAssemblyStateCI(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitiveRestartEnable)
{
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
	pipelineInputAssemblyStateCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.topology               = topology;
	pipelineInputAssemblyStateCreateInfo.flags                  = flags;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
	return pipelineInputAssemblyStateCreateInfo;
}

VkPipelineDynamicStateCreateInfo Vk::GetDynamicStateCI(const std::vector<VkDynamicState> &dynamic_states, VkPipelineDynamicStateCreateFlags flags)
{
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
	pipelineDynamicStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineDynamicStateCreateInfo.pDynamicStates    = dynamic_states.data();
	pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	pipelineDynamicStateCreateInfo.flags             = flags;
	return pipelineDynamicStateCreateInfo;
}

VkDescriptorPoolSize Vk::GetOneDescriptorPoolSizeDescription(VkDescriptorType type, uint32_t descriptor_count)
{
	return {type, descriptor_count};
}

VkDescriptorPoolCreateInfo Vk::GetDescriptorPoolCI(const std::vector<VkDescriptorPoolSize> &pool_sizes, uint32_t max_sets, const void *pNext, VkDescriptorPoolCreateFlags flags)
{
	VkDescriptorPoolCreateInfo descriptor_pool_CI{};
	descriptor_pool_CI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

	descriptor_pool_CI.pNext = pNext;
	descriptor_pool_CI.flags = flags;

	descriptor_pool_CI.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	descriptor_pool_CI.pPoolSizes    = pool_sizes.data();
	descriptor_pool_CI.maxSets       = max_sets;

	return descriptor_pool_CI;
}

VkDescriptorSetLayoutBinding Vk::GetDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlagBits stage_flags, uint32_t descriptor_count, const VkSampler *pImmutableSamplers)
{
	VkDescriptorSetLayoutBinding setLayoutBinding{};
	setLayoutBinding.descriptorType     = type;
	setLayoutBinding.stageFlags         = stage_flags;
	setLayoutBinding.binding            = binding;
	setLayoutBinding.descriptorCount    = descriptor_count;
	setLayoutBinding.pImmutableSamplers = pImmutableSamplers;

	return setLayoutBinding;
}

VkDescriptorSetLayoutCreateInfo Vk::GetDescriptorSetLayoutCI(const std::vector<VkDescriptorSetLayoutBinding> &bindings, const void *pNext, VkDescriptorSetLayoutCreateFlags flags)
{

	//TODO:	use perfect forwarding to check bindings is not a rvalue;
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings    = bindings.data();
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	descriptorSetLayoutCreateInfo.pNext        = pNext;
	descriptorSetLayoutCreateInfo.flags        = flags;
	return descriptorSetLayoutCreateInfo;
}

VkDescriptorSetAllocateInfo Vk::GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const VkDescriptorSetLayout &SetLayout)
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool     = descriptorPool;
	descriptorSetAllocateInfo.pSetLayouts        = &SetLayout;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	return descriptorSetAllocateInfo;
}

VkDescriptorSetAllocateInfo Vk::GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const std::vector<VkDescriptorSetLayout> &SetLayouts)
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool     = descriptorPool;
	descriptorSetAllocateInfo.pSetLayouts        = SetLayouts.data();
	descriptorSetAllocateInfo.descriptorSetCount = (uint32_t) SetLayouts.size();
	return descriptorSetAllocateInfo;
}




size_t Vk::GetSamplerUUID_unsafe()
{
	return ++UUID_safe<Vk::Type::Sampler>;
}
