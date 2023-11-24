#pragma once
#include "VkRenderpassManager.h"
#include "VkTexture.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace RenderGraphV0
{
class PassNode;
}


template <typename RESOURCE>
class VirtualResource
{
  public:
	enum class RsrcType
	{
		Imported,
		Established
	};

  public:
	VirtualResource(std::string name_, std::shared_ptr<RESOURCE> ptr_rsrc_) :

	    name(name_), ptr_rsrc(ptr_rsrc_), descriptor(std::nullopt), rsrc_type(RsrcType::Imported)
	{
	}

	VirtualResource(std::string name_, typename RESOURCE::Descriptor descriptor_) :
	    name(name_), ptr_rsrc(nullptr), descriptor(descriptor_), rsrc_type(RsrcType::Established)
	{
	}



	VirtualResource() = delete;

	VirtualResource(const VirtualResource &) = delete;
	VirtualResource &operator=(const VirtualResource &) = delete;

	VirtualResource(VirtualResource &&) = delete;
	VirtualResource &operator=(VirtualResource &&) = delete;

	~VirtualResource() = default;

  public:
	void Actualize(VkRenderpassManager &);
	void Recycle(VkRenderpassManager &);
	void InsertSyncIntoCmdBuf(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync);


  public:
	std::string                                  name;
	std::shared_ptr<RESOURCE>                    ptr_rsrc;
	std::optional<typename RESOURCE::Descriptor> descriptor;
	RsrcType                                     rsrc_type;
	std::vector<RenderGraphV0::PassNode *>       passes_access_this_rsrc;
};



//
//
//Rsrc Actualization spcialization



template <>
inline void VirtualResource<VkTexture>::Actualize(VkRenderpassManager &renderpass_manager)
{
	if (!descriptor.has_value())
	{
		throw std::runtime_error("No descriptor is provided.");
	}

	const auto &dis = descriptor.value();
	ptr_rsrc        = renderpass_manager.GetTextureFactory().ProduceUnfilledTexture(*dis.tex_img_PP, dis.sample_CI, dis.img_view_CI);
}



template <>
inline void VirtualResource<VkBufferBase>::Actualize(VkRenderpassManager &renderpass_manager)
{
	if (!descriptor.has_value())
	{
		throw std::runtime_error("No descriptor is provided.");
	}

	const auto &dis = descriptor.value();

	ptr_rsrc = renderpass_manager.GetBufferFactory().ProduceBuffer(dis.buffer_size, descriptor.value());
}


template <>
inline void VirtualResource<VkTexture>::Recycle(VkRenderpassManager &renderpass_manager)
{
	ptr_rsrc.reset();
}


template <>
inline void VirtualResource<VkBufferBase>::Recycle(VkRenderpassManager &renderpass_manager)
{
	ptr_rsrc.reset();
}



template <>
inline void VirtualResource<VkTexture>::InsertSyncIntoCmdBuf(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync)
{
	const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	    .srcAccessMask = source_syn.access_mask,
	    .srcStageMask  = source_syn.stage_mask,
	    .oldLayout     = source_syn.layout_inpass,

	    .dstAccessMask = target_sync.access_mask,
	    .dstStageMask  = target_sync.stage_mask,
	    .newLayout     = target_sync.layout_inpass,
	    //TODO: sub ranges
	    .subresource_range = std::nullopt,
	};
	ptr_rsrc->InsertImageMemoryBarrier(cmd_buf, image_memory_barrier_enhanced);
}





template <>
inline void VirtualResource<VkBufferBase>::InsertSyncIntoCmdBuf(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync)
{
	//TODO:
}




