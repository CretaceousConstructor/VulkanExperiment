#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "ModelLoader.h"
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkTexture.h"
#include "VkUniformBuffer.h"
#include "VkWindows.h"
#include "VkMetaInfo.h"
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <utility>
#include <vector>
#include <xhash>

//class DescMetaInfo
//{
//public:
//	//DescMetaInfo(
//	//    uint8_t _pass,
//	//    uint8_t _subpass,
//	//    uint8_t _set
//	//);
//	uint8_t pass;
//	uint8_t subpass;
//	uint8_t set;
//
//	bool operator==(const DescMetaInfo &other) const
//	{
//		return (pass == other.pass && subpass == other.subpass && set == other.set);
//	}
//
//
//};
//
//namespace std
//{
//	template <>
//	struct hash<DescMetaInfo>
//	{
//		size_t operator()(const DescMetaInfo  &info) const noexcept
//		{
//			return ((hash<glm::uint8_t>()(info.pass) ^
//					 (hash<glm::uint8_t>()(info.subpass) << 1)) >>
//					0) ^
//				   (hash<glm::uint8_t>()(info.set) << 1);
//		}
//	};
//}

class VkDescriptorManager
{
  public:
  public:
	//TODO:VkDescriptorSet不用进行调用destroy类API，但是如何回收回到pool中，这个功能暂时不进行实现，
	VkDescriptorManager(VkDeviceManager &_device_manager);

	VkDescriptorManager(const VkDescriptorManager &) = delete;
	VkDescriptorManager &operator=(const VkDescriptorManager &) = delete;

	VkDescriptorManager(VkDescriptorManager &&) = delete;
	VkDescriptorManager &operator=(VkDescriptorManager &&) = delete;

	~VkDescriptorManager();

	void AddDescriptorPool(		const DescriptorMetaInfo pool_meta_info, std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs, uint32_t max_sets);
	void AddDescriptorSetLayout(const DescriptorMetaInfo set_layout_meta_info, std::vector<VkDescriptorSetLayoutBinding> LayoutBindings);
	void AddDescriptorSetBundle(const DescriptorMetaInfo pool_meta_info, const DescriptorMetaInfo set_layout_meta_info, size_t num_in_flight);

	void UpdateDescriptorSet(std::vector<VkWriteDescriptorSet> write_descriptor_sets, const DescriptorMetaInfo set_meta_info, size_t frame_inflight) const;

	template <class Resource>
	void UpdateDescriptorSet(Resource &resource, const DescriptorMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement);

	const VkDescriptorPool &     GetPool(const DescriptorMetaInfo pool_meta_info);
	const VkDescriptorSetLayout &GetSetLayout(const DescriptorMetaInfo set_layout_meta_info);

	[[nodiscard]] std::vector<VkDescriptorSetLayout> SearchLayout(const DescriptorMetaInfo set_layout_meta_info) const;
	[[nodiscard]] std::vector<VkDescriptorSetLayout> SearchLayout(const PipelineMetaInfo set_layout_meta_info) const;

  private:
	VkDeviceManager &device_manager;

  private:
	class Pool
	{
	  public:
		std::vector<std::pair<VkDescriptorType, uint32_t>> info;
		uint32_t                                           max_sets;
		VkDescriptorPool                                   descriptor_pool;
	};
	class SetLayOut
	{
	  public:
		std::vector<VkDescriptorSetLayoutBinding> LayoutBindings;
		VkDescriptorSetLayout                     set_layout;
	};

  private:
	std::unordered_map<DescriptorMetaInfo, Pool>                         descriptor_pools;
	std::unordered_map<DescriptorMetaInfo, SetLayOut>                    set_layouts;
	std::unordered_map<DescriptorMetaInfo, std::vector<VkDescriptorSet>> descriptor_sets;
};

template <>
inline void VkDescriptorManager::UpdateDescriptorSet<UniformBufferBundle>(UniformBufferBundle& resource, const DescriptorMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement)
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}
	const auto &set_bundle = descriptor_sets[set_meta_info];
	for (uint8_t frame_inflight = 0; frame_inflight < set_bundle.size(); frame_inflight++)
	{

		auto write_set   = resource[frame_inflight].GetWriteDescriptorSetInfo(dstbinding, dstArrayElement);
		write_set.dstSet = set_bundle[frame_inflight];
		vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}


}

template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkTexture>(VkTexture &resource, const DescriptorMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement)
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}
	const auto &set_bundle = descriptor_sets[set_meta_info];
	for (size_t frame_inflight = 0; frame_inflight < set_bundle.size(); frame_inflight++)
	{
		auto write_set   = resource.GetWriteDescriptorSetInfo(dstbinding, dstArrayElement);
		write_set.dstSet = set_bundle[frame_inflight];
		vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}


}
