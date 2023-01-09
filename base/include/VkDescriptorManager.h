#pragma once

#include "VK.h"
#include "VkBufferBase.h"
#include "VkGraphicsComponent.h"
#include "VkMetaInfo.h"
#include "VkTexture.h"

#include <unordered_map>
#include <vector>

//class VkDescriptorSetBundle
//{
//  public:
//	VkDescriptorSetBundle(std::vector<VkDescriptorSet> descriptor_set_bundle_) :
//	    bundle_count(static_cast<uint32_t>(descriptor_set_bundle_.size())), descriptor_set_bundle(std::move(descriptor_set_bundle_))
//	{
//	}
//
//	~VkDescriptorSetBundle() = default;
//
//	VkDescriptorSetBundle()                              = delete;
//	VkDescriptorSetBundle(const VkDescriptorSetBundle &) = delete;
//	VkDescriptorSetBundle &operator=(const VkDescriptorSetBundle &) = delete;
//
//	VkDescriptorSetBundle(VkDescriptorSetBundle &&) = delete;
//	VkDescriptorSetBundle &operator=(VkDescriptorSetBundle &&) = delete;
//
//	[[nodiscard]] uint32_t               GetBundleCount() const;
//	const VkDescriptorSet &              operator[](size_t index) const;
//	[[nodiscard]] const VkDescriptorSet &GetOne(size_t index) const;
//
//	[[nodiscard]] const std::vector<VkDescriptorSet> &GetDescriptorSetArray() const;
//
//  private:
//	uint32_t                     bundle_count{};
//	std::vector<VkDescriptorSet> descriptor_set_bundle;
//};

class VkDescriptorSetFactory
{
  public:
	VkDescriptorSetFactory(VkGraphicsComponent &gfx_)

	    :
	    gfx(gfx_), device_manager(gfx.DeviceMan())
	{
	}
	using DescriptorSetBundle = std::vector<VkDescriptorSet>;

	VkDescriptorSet ProduceDescriptorSet(VkDescriptorPool descriptor_pool, const VkDescriptorSetLayout set_layout) const
	{
		VkDescriptorSet result;
		//ALLOCATE DESCRIPTORS
		const VkDescriptorSetAllocateInfo alloc_info = Vk::GetDescriptorAllocateInfo(descriptor_pool, set_layout);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &alloc_info, &result));
		return result;
	}

	//std::shared_ptr<VkDescriptorSetBundle> ProduceDescriptorSetBundlePtr(VkDescriptorPool descriptor_pool, const VkDescriptorSetLayout set_layout, uint32_t bundle_size) const
	//{
	//	const std::vector<VkDescriptorSetLayout> layouts{bundle_size, set_layout};
	//	std::vector<VkDescriptorSet>             result{bundle_size, nullptr};

	//	//ALLOCATE DESCRIPTORS
	//	const VkDescriptorSetAllocateInfo alloc_info = Vk::GetDescriptorAllocateInfo(descriptor_pool, layouts);
	//	VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &alloc_info, result.data()))
	//	return std::make_shared<VkDescriptorSetBundle>(std::move(result));
	//}

	std::vector<VkDescriptorSet> ProduceDescriptorSetBundle(VkDescriptorPool descriptor_pool, const VkDescriptorSetLayout set_layout, uint32_t bundle_size) const
	{
		const std::vector<VkDescriptorSetLayout> layouts{bundle_size, set_layout};
		std::vector<VkDescriptorSet>             result{bundle_size, nullptr};

		//ALLOCATE DESCRIPTORS
		const VkDescriptorSetAllocateInfo alloc_info = Vk::GetDescriptorAllocateInfo(descriptor_pool, layouts);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &alloc_info, result.data()))
		return result;
	}

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};

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

	void                                       AddDescriptorPool(const DescriptorPoolMetaInfo pool_meta_info, std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs, uint32_t max_sets);
	[[nodiscard]] const VkDescriptorPool &     GetPool(const DescriptorPoolMetaInfo pool_meta_info) const;
	void                                       AddDescriptorSetLayout(const DescriptorSetLayoutMetaInfo set_layout_meta_info, std::vector<VkDescriptorSetLayoutBinding> LayoutBindings);
	[[nodiscard]] const VkDescriptorSetLayout &GetSetLayout(const DescriptorSetLayoutMetaInfo set_layout_meta_info) const;
	void                                       AddDescriptorSetBundle(const DescriptorSetMetaInfo bundle_set_meta_info, size_t num_of_bundle);
	const std::vector<VkDescriptorSet> &       GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const;

	void UpdateDescriptorSet(std::vector<VkWriteDescriptorSet> write_descriptor_sets, const DescriptorSetMetaInfo set_meta_info, size_t frame_inflight) const;

	template <class Resource>
	void UpdateDescriptorSet(Resource &resource, const DescriptorSetMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement = 0);

	template <class Resource>
	static void UpdateDescriptorSet(const VkDevice device, Resource &resource, const std::vector<VkDescriptorSet> &set_bundle, uint32_t dstbinding, uint32_t dstArrayElement = 0);

	template <class Resource>
	static void UpdateDescriptorSet(const VkDevice device, Resource &resource, const VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement = 0);

	//TODO:
	[[nodiscard]] std::vector<VkDescriptorSetLayout> SearchLayout(const std::vector<DescriptorSetLayoutMetaInfo> &set_layout_meta_info) const;
	//[[nodiscard]] std::vector<VkDescriptorSetLayout> SearchLayout(const PipelineMetaInfo set_layout_meta_info) const;

	//********************************************************************
	static VkDescriptorPoolSize       GetOneDescriptorPoolSizeDescription(VkDescriptorType type, uint32_t descriptor_count);
	static VkDescriptorPoolCreateInfo GetDescriptorPoolCI(const std::vector<VkDescriptorPoolSize> &pool_sizes, uint32_t max_sets);

	static VkDescriptorSetLayoutBinding    GetDescriptorLayoutBindingDescriptions(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t descriptor_count = 1);
	static VkDescriptorSetLayoutCreateInfo GetDescriptorSetLayoutCI(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

	static VkDescriptorSetAllocateInfo GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const VkDescriptorSetLayout &SetLayout);
	static VkDescriptorSetAllocateInfo GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const std::vector<VkDescriptorSetLayout> &SetLayouts);
	//********************************************************************

  private:
	VkGraphicsComponent &  gfx;
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
	std::unordered_map<DescriptorSetLayoutMetaInfo, SetLayOut>              set_layouts;
	std::unordered_map<DescriptorPoolMetaInfo, Pool>                        descriptor_pools;
	std::unordered_map<DescriptorSetMetaInfo, std::vector<VkDescriptorSet>> descriptor_sets;
};



//seems to be deprecated
template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkBufferBase::BufferPtrBundle>(VkBufferBase::BufferPtrBundle &resource, const DescriptorSetMetaInfo set_meta_info, uint32_t dstbinding, uint32_t dstArrayElement)
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}
	const auto &set_bundle = descriptor_sets[set_meta_info];
	for (uint32_t img_indx = 0; img_indx < set_bundle.size(); img_indx++)
	{
		auto write_set   = resource[img_indx]->GetWriteDescriptorSetInfo(dstbinding, dstArrayElement);
		write_set.dstSet = set_bundle[img_indx];
		vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}
}
//seems to be depricated
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
}

//n to n update buffer
template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkBufferBase::BufferPtrBundle>(const VkDevice device, VkBufferBase::BufferPtrBundle &resource, const std::vector<VkDescriptorSet> &set_bundle, uint32_t dstbinding, uint32_t dstArrayElement)
{
	assert(set_bundle.size() == resource.size());

	for (size_t i = 0; i < set_bundle.size(); i++)
	{
		auto write_set   = resource[i]->GetWriteDescriptorSetInfo(dstbinding);
		write_set.dstSet = set_bundle[i];
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}
}

//1 to 1 update buffer 
template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkBufferBase>(const VkDevice device, VkBufferBase &resource, const VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement)
{
	auto write_set   = resource.GetWriteDescriptorSetInfo(dstbinding);
	write_set.dstSet = set;
	vkUpdateDescriptorSets(device, static_cast<uint32_t>(1), &write_set, 0, nullptr);
}

//1 to n update texture
template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkTexture>(const VkDevice device, VkTexture &resource, const std::vector<VkDescriptorSet> &set_bundle, uint32_t dstbinding, uint32_t dstArrayElement)
{

	for (size_t img_indx = 0; img_indx < set_bundle.size(); img_indx++)
	{
		auto write_set   = resource.GetWriteDescriptorSetInfo(dstbinding);
		write_set.dstSet = set_bundle[img_indx];
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(1), &write_set, 0, nullptr);
	}
}

//1 to 1 update texture
template <>
inline void VkDescriptorManager::UpdateDescriptorSet<VkTexture>(const VkDevice device, VkTexture &resource, const VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement)
{
	auto write_set   = resource.GetWriteDescriptorSetInfo(dstbinding);
	write_set.dstSet = set;
	vkUpdateDescriptorSets(device, static_cast<uint32_t>(1), &write_set, 0, nullptr);
}
