#include "VkMetaInfo.h"

//namespace std
//{
//template <>
//struct hash<DescriptorPoolMetaInfo>
//{
//	size_t operator()(const DescriptorPoolMetaInfo &info) const noexcept
//	{
//		return (hash<uint32_t>()(info.thread_id));
//	}
//};
//
//template <>
//struct hash<DescriptorSetLayoutMetaInfo>
//{
//	size_t operator()(const DescriptorSetLayoutMetaInfo &info) const noexcept
//	{
//		return (hash<uint32_t>()(info.id));
//	}
//};
//
//template <>
//struct hash<DescriptorSetMetaInfo>
//{
//	size_t operator()(const DescriptorSetMetaInfo &info) const noexcept
//	{
//		return hash<uint32_t>()(info.id) ^ ((hash<DescriptorSetLayoutMetaInfo>()(info.layout) << 1)>>1);
//	}
//};
//
//
////==============================================================================
//
//template <>
//struct hash<PipelineLayoutMetaInfo>
//{
//	size_t operator()(const PipelineLayoutMetaInfo &info) const noexcept
//	{
//		return (hash<glm::uint32_t>()(info.id));
//	}
//};
//
//
//
//template <>
//struct hash<PipelineMetaInfo>
//{
//	size_t operator()(const PipelineMetaInfo &info) const noexcept
//	{
//		return ((hash<glm::uint32_t>()(info.pass) ^
//		        (hash<glm::uint32_t>()(info.subpass) << 1)) >> 1) ^
//		        (hash<PipelineLayoutMetaInfo>()(info.pipelayout_id) << 1);
//	}
//};
//
//
//template <>
//struct hash<ShaderMetaInfo>
//{
//	size_t operator()(const ShaderMetaInfo &info) const noexcept
//	{
//		return ((hash<std::string>()(info.shader_path) ^
//		         (hash<VkShaderStageFlagBits>()(info.shader_binding_stage) << 1)) >> 1);
//	}
//};
//
//
////==============================================================================
//
//}
//
//
//
//
