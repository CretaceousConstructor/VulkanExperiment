#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "ModelLoader.h"
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkTexture.h"
#include "VkWindows.h"
#include "VkUniformBuffer.h"
#include "VkTexture.h"
#include <unordered_map>
#include <utility>
#include <vector>
#include <glm/gtx/hash.hpp>
#include <xhash>

class DescMetaInfo
{
public:
	//DescMetaInfo(
	//    uint8_t _pass,
	//    uint8_t _subpass,
	//    uint8_t _set
	//);
	uint8_t pass;
	uint8_t subpass;
	uint8_t set;

	bool operator==(const DescMetaInfo &other) const
	{
		return (pass == other.pass && subpass == other.subpass && set == other.set);
	}


};

namespace std
{
	template <>
	struct hash<DescMetaInfo>
	{
		size_t operator()(const DescMetaInfo  &info) const noexcept 
		{
			return ((hash<glm::uint8_t>()(info.pass) ^
					 (hash<glm::uint8_t>()(info.subpass) << 1)) >>
					0) ^
				   (hash<glm::uint8_t>()(info.set) << 1);
		}
	};
}    


class VkDescriptorManager
{

public:


  public:
	//TODO:VkDescriptorSet不用进行调用destroy类API，但是如何回收回到pool中，这个功能暂时不进行实现，
	VkDescriptorManager(VkDeviceManager &_device_manager);


	VkDescriptorManager(const VkDescriptorManager &) = delete;
	VkDescriptorManager& operator=(const VkDescriptorManager&) = delete;

	VkDescriptorManager(VkDescriptorManager &&) = delete;
	VkDescriptorManager& operator=(VkDescriptorManager&&) = delete;


	~VkDescriptorManager();

	void AddDescriptorPool(const DescMetaInfo pool_meta_info, std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs, uint32_t max_sets);
	void AddDescriptorSetLayout(const DescMetaInfo set_layout_meta_info,std::vector<VkDescriptorSetLayoutBinding> LayoutBindings);
	void AddDescriptorSetBundle(const DescMetaInfo pool_meta_info, const DescMetaInfo set_layout_meta_info,size_t num_in_flight);




	void UpdateDescriptorSet(std::vector<VkWriteDescriptorSet> write_descriptor_sets,const DescMetaInfo set_meta_info,size_t frame_inflight);



	template <class Resource>
	void UpdateDescriptorSet(Resource& resource,const DescMetaInfo set_meta_info,uint32_t dstbinding,uint32_t dstArrayElement);





	const VkDescriptorPool& GetPool(const DescMetaInfo pool_meta_info);
	const VkDescriptorSetLayout& GetSetLayout(const DescMetaInfo set_layout_meta_info);

	[[nodiscard]] std::vector<VkDescriptorSetLayout>	SearchLayout(const DescMetaInfo set_layout_meta_info);


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
	std::unordered_map<DescMetaInfo, Pool> descriptor_pools;
	std::unordered_map<DescMetaInfo, SetLayOut> set_layouts;
	std::unordered_map<DescMetaInfo,std::vector<VkDescriptorSet>> descriptor_sets;



};





template <>
inline void VkDescriptorManager::UpdateDescriptorSet< std::vector<std::unique_ptr<VkUniformBuffer>> >(std::vector<std::unique_ptr<VkUniformBuffer>>& resource,const DescMetaInfo set_meta_info,uint32_t dstbinding,uint32_t dstArrayElement)
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}
	const auto &set_bundle = descriptor_sets[set_meta_info];
	for (size_t frame_inflight = 0 ; frame_inflight<set_bundle.size();frame_inflight++ )
	{
		auto write_set = resource[frame_inflight]->GetWriteDescriptorSetInfo(dstbinding, dstArrayElement);
		write_set.dstSet = set_bundle[frame_inflight];
		vkUpdateDescriptorSets(device_manager.GetLogicalDeviceRef(), static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}
}


template <>
inline void VkDescriptorManager::UpdateDescriptorSet< std::unique_ptr<VkTexture> >(std::unique_ptr<VkTexture>& resource,const DescMetaInfo set_meta_info,uint32_t dstbinding,uint32_t dstArrayElement)
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}
	const auto &set_bundle = descriptor_sets[set_meta_info];
	for (size_t frame_inflight = 0 ; frame_inflight<set_bundle.size();frame_inflight++ )
	{
		auto write_set = resource->GetWriteDescriptorSetInfo(dstbinding, dstArrayElement);
		write_set.dstSet = set_bundle[frame_inflight];
		vkUpdateDescriptorSets(device_manager.GetLogicalDeviceRef(), static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}
}

