#pragma once
#include "VkGraphicsComponent.h"
#include "VkTexture.h"
#include "VkBufferBundle.h" 
#include "VkMetaInfo.h"
#include <unordered_map>
#include <vector>





class VkDescriptorManager
{
  public:
	//TODO:VkDescriptorSet不用进行调用destroy类API，但可以用vkFree回收到pool中，这个功能暂时不进行实现，
	VkDescriptorManager(VkGraphicsComponent &_gfx);

	VkDescriptorManager(const VkDescriptorManager &) = delete;
	VkDescriptorManager &operator=(const VkDescriptorManager &) = delete;

	VkDescriptorManager(VkDescriptorManager &&) = delete;
	VkDescriptorManager &operator=(VkDescriptorManager &&) = delete;

	~VkDescriptorManager();





	void AddDescriptorPool(const DescriptorPoolMetaInfo pool_meta_info, std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs, uint32_t max_sets);
	[[nodiscard]] const VkDescriptorPool& GetPool(const DescriptorPoolMetaInfo pool_meta_info) const;
	void AddDescriptorSetLayout(const DescriptorSetLayoutMetaInfo set_layout_meta_info, std::vector<VkDescriptorSetLayoutBinding> LayoutBindings);
	[[nodiscard]]const VkDescriptorSetLayout& GetSetLayout(const DescriptorSetLayoutMetaInfo set_layout_meta_info) const;
	void AddDescriptorSetBundle(const DescriptorSetMetaInfo bundle_set_meta_info, size_t num_of_bundle);
	const std::vector<VkDescriptorSet> &      GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const;

	void UpdateDescriptorSet(std::vector<VkWriteDescriptorSet> write_descriptor_sets, const DescriptorSetMetaInfo set_meta_info, size_t frame_inflight) ;
	template <class Resource>
	void UpdateDescriptorSet(Resource &resource, const DescriptorSetMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement);





	//TODO:
	[[nodiscard]] std::vector<VkDescriptorSetLayout> SearchLayout(const std::vector<DescriptorSetLayoutMetaInfo>& set_layout_meta_info) const;
	[[nodiscard]] std::vector<VkDescriptorSetLayout> SearchLayout(const PipelineMetaInfo set_layout_meta_info) const;




  private:
	VkGraphicsComponent &gfx;
	const VkDeviceManager &device_manager;

  private:
	class Pool
	{
	  public:
		std::vector<std::pair<VkDescriptorType, uint32_t>> info;
		uint32_t                                           max_sets{};
		VkDescriptorPool                                   descriptor_pool{};
	};
	class SetLayOut
	{
	  public:
		std::vector<VkDescriptorSetLayoutBinding> LayoutBindings;
		VkDescriptorSetLayout                     set_layout{};
	};



  private:
	std::unordered_map<DescriptorSetLayoutMetaInfo, SetLayOut>                  set_layouts;
	std::unordered_map<DescriptorPoolMetaInfo, Pool>                         descriptor_pools;
	std::unordered_map<DescriptorSetMetaInfo, std::vector<VkDescriptorSet>> descriptor_sets;



};

template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkBufferBundle>(VkBufferBundle& resource, const DescriptorSetMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement)
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}
	const auto &set_bundle = descriptor_sets[set_meta_info];
	for (uint32_t frame_inflight = 0; frame_inflight < set_bundle.size(); frame_inflight++)
	{

		auto write_set   = resource[frame_inflight].GetWriteDescriptorSetInfo(dstbinding, dstArrayElement);
		write_set.dstSet = set_bundle[frame_inflight];
		vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}


}

template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkTexture>(VkTexture &resource, const DescriptorSetMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement)
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




		////subpass0
		//{
		//	for (size_t frame_inflight = 0; frame_inflight < swapchain_manager.GetSwapImageCount(); frame_inflight++)
		//	{
		//		std::vector<VkWriteDescriptorSet> writeDescriptorSets;
		//		//SET INFOS
		//		/*
		//			Set 0,Binding 0: VS matrices Uniform buffer,amount = 1
		//		*/
		//		writeDescriptorSets.emplace_back(buffers[frame_inflight]->GetWriteDescriptorSetInfo(0, 0));
		//		/*
		//			Set 0,Binding 1: FS texture image and sampler,amount = 1
		//		*/
		//		writeDescriptorSets.emplace_back(ktx_texure->GetWriteDescriptorSetInfo(1, 0));

		//		descriptor_manager->UpdateDescriptorSet(std::move(writeDescriptorSets), set_layout_meta_info, frame_inflight);
		//	}
		//};


}
