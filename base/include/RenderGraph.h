#pragma once
#include "VkGltfModel.h"
#include "VkRenderpassBaseRG.h"
#include <array>

//TODO:功能实现的差不多以后，想办法解耦合。

namespace RenderGraph
{
struct RsrcUsageInfoSlot
{
	using Handle = size_t;
	Handle handle{};
};

struct ResourceSlot
{
	using Handle = size_t;
	Handle resource_handle{0};        //资源 在资源数组中的下标
	Handle node_handle{0};            //资源 对应的资源节点Node在数组中的下标

	bool operator==(const ResourceSlot &other) const
	{
		return (resource_handle == other.resource_handle && node_handle == other.node_handle);
	}
};

}        // namespace RenderGraph

namespace std
{
template <>
struct std::hash<RenderGraph::ResourceSlot>
{
	size_t operator()(const RenderGraph::ResourceSlot &k) const noexcept
	{
		using std::hash;
		return ((hash<RenderGraph::ResourceSlot::Handle>()(k.resource_handle) ^ (hash<RenderGraph::ResourceSlot::Handle>()(k.node_handle) << 1)));
	}
};

}        // namespace std

namespace RenderGraph
{
class DependencyGraph;
class PassNode;
class GraphicsPassNode;
class ResourceNode;
class VirtualResource;
class ResourceEdge;

template <typename RESOURCE>
class Resource;

class DependencyGraph
{
  public:
	using PassNodeHandle  = size_t;
	using RsrcUsageHandle = size_t;

	class Edge
	{
	  protected:
	  public:
		Edge(PassNodeHandle f, PassNodeHandle t);
		~Edge() = default;

		Edge(const Edge &) = default;
		Edge(Edge &&)      = default;

		Edge &operator=(Edge &&) = delete;
		Edge &operator=(const Edge &) = delete;

	  public:
		const PassNodeHandle from;
		const PassNodeHandle to;
	};

	class Node
	{
	  public:
		void AddOutGoingEdge(Edge e);
		void AddInComingEdge(Edge e);

	  public:
		Node(size_t node_handle_);

		virtual ~Node() = default;

		Node(const Node &) = delete;
		Node &operator=(const Node &) = delete;

		Node(Node &&) = delete;
		Node &operator=(Node &&) = delete;

	  public:
		std::vector<Edge> incoming_edges;
		std::vector<Edge> outgoing_edges;

		uint32_t ref_count{0};
		size_t   node_handle{};
	};

	class ResourceNode : public Node
	{
	  public:
		ResourceNode();
		~ResourceNode() override = default;

		ResourceNode(const ResourceNode &) = delete;
		ResourceNode &operator=(const ResourceNode &) = delete;
		ResourceNode(ResourceNode &&)                 = delete;
		ResourceNode &operator=(ResourceNode &&) = delete;

		std::vector<std::pair<size_t, size_t>> pass_accessing_order;
	};

  public:
	//清楚所有
	void Purge();
	//FrameGraph根据前面Setup的VirtualResource跟各个passNode之间的引用关系(read/write)剔除无引用的VirtualResource ，记录VirtualResource 从哪里开始使用，从哪里开始结束，用firstPassNode和lastPassNode记录。(VirtualResource的生命周期)
	[[nodiscard]] bool Compile();
	[[nodiscard]] bool Execute(VkCommandBuffer);        //此函数中生成真正的资源

  public:
	PassNodeHandle AddGfxPassNode(std::shared_ptr<VkRenderpassBaseRG> renderpass, const std::string name, const std::vector<ResourceSlot> inputs, const std::vector<ResourceSlot> putputs);

	template <typename RESOURCE>
	ResourceSlot AddResourceNode(std::unique_ptr<Resource<RESOURCE>> rsrc, std::string name);

	void RegisterRsrcsUsage(const PassNodeHandle pass_node_handle, const ResourceSlot rsrc_slot, std::unique_ptr<RsrcUsageInfoInPass> rsrc_info);
	void ExecutePass(VkCommandBuffer cmd, const PassNodeHandle pass);
	bool Move();

  public:
	DependencyGraph(VkRenderpassManager &renderpass_manager_);

	DependencyGraph()  = delete;
	~DependencyGraph() = default;

	DependencyGraph(const DependencyGraph &) = delete;
	DependencyGraph &operator=(const DependencyGraph &) = delete;
	DependencyGraph(DependencyGraph &&)                 = delete;
	DependencyGraph &operator=(DependencyGraph &&) = delete;

  private:
	[[nodiscard]] bool IfPassInOrder(const PassNodeHandle fir, const PassNodeHandle sec) const;
	void               GeneratePassNodeDepen(std::vector<std::unordered_set<size_t>> &pass_node_dependencies) const;

	VirtualResource &GetResource(const ResourceSlot &slot);
	ResourceNode &   GetResourceNode(const ResourceSlot &slot);

  public:
	VkAttachmentInfo  GetAttachmentRsrc(std::string name, PassNodeHandle pass_node_handle);
	VkTexUsageInfo    GetTextureRsrc(std::string name, PassNodeHandle pass_node_handle);
	VkUniBufUsageInfo GetBufferRsrc(std::string name, PassNodeHandle pass_node_handle);

  public:
	//ResourceSlot是用来记录VirtualResource和ResourceNode的一一对应关系。
	std::vector<ResourceSlot> slots_of_rsrcs;

	//VirtualResource
	std::vector<std::unique_ptr<VirtualResource>> resources;
	std::vector<std::unique_ptr<ResourceNode>> nodes_of_resources;
	std::vector<std::unique_ptr<PassNode>>     nodes_of_passes;

	//Resources usage within passes，声明各个pass中，不同资源的使用方式
	std::vector<std::unique_ptr<RsrcUsageInfoInPass>> rsrc_usage_infos;

	//资源map
	std::unordered_map<std::string, ResourceSlot> rsrcs_map;

	//资源使用方式记录
	std::unordered_map<ResourceSlot, std::array<std::pair<std::optional<PassNodeHandle>, RsrcUsageInfoSlot>, 2>> resources_usage_recording_table;

  private:
	std::vector<PassNodeHandle> final_execution_order;

  private:
	VkRenderpassManager &renderpass_manager;
};

template <typename RESOURCE>
ResourceSlot DependencyGraph::AddResourceNode(std::unique_ptr<Resource<RESOURCE>> rsrc, std::string name)
{
	//provides slot<--resources -->stores actual resource
	resources.push_back(std::move(rsrc));
	const auto &rsrc_node = nodes_of_resources.emplace_back(std::make_unique<ResourceNode>());

	//thread safety, forget it!
	const ResourceSlot slot{
	    .resource_handle = resources.size() - 1,
	    .node_handle     = nodes_of_resources.size() - 1,
	};

	rsrc_node->node_handle = slot.node_handle;

	slots_of_rsrcs.push_back(slot);

	rsrcs_map.emplace(name, slot);        //把资源的名字和 slot对应起来，方便之后具体的pass在使用资源时 取用。

	return slot;
}
}        // namespace RenderGraph

namespace RenderGraph
{
class VirtualResource
{
  public:
	enum class RsrcAccessType
	{
		Read,
		Write,
		ReadWrite,
		WriteRead
	};

	enum class RsrcLifeTimeType

	{
		Persistent,        //imported outside
		Transient,
		CrossMultiFrames
	};

  public:
	virtual void Actualize(VkRenderpassManager &renderpass_manager) = 0;
	virtual void DeActualize(VkRenderpassManager &)                                                                   = 0;
	virtual void StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, RsrcUsageInfoInPass &)        = 0;
	virtual void InsertSync(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync) = 0;

  public:
	VirtualResource(RsrcLifeTimeType life_time_);

  public:
	VirtualResource()          = delete;
	virtual ~VirtualResource() = default;

	VirtualResource(const VirtualResource &) = delete;
	VirtualResource &operator=(const VirtualResource &) = delete;

	VirtualResource(VirtualResource &&) = delete;
	VirtualResource &operator=(VirtualResource &&) = delete;

  public:
	const RsrcLifeTimeType life_time;
	uint32_t               ref_count = 0;        //refcount > 1才能申请到资源，否则就等于说被剔除了
};

//这是个模板类，但是通过ResourcesSlot这种indirection，就可以实现间接引用资源而不用specify一堆类型变量
template <typename RESOURCE>
class Resource final : public VirtualResource        //之后会获得GPU资源的类,但不是一开始就可以得到GPU资源
{
  public:
	using Descriptor = typename RESOURCE::Descriptor;

  public:
	void Actualize(VkRenderpassManager &) override;
	void DeActualize(VkRenderpassManager &) override;
	void StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, RsrcUsageInfoInPass &) override;
	void InsertSync(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync) override;

  public:
	explicit Resource(Descriptor dstor);
	explicit Resource(std::shared_ptr<RESOURCE> rsrc);

	~Resource() override = default;

  public:
	Resource()                 = delete;
	Resource(const Resource &) = delete;
	Resource &operator=(const Resource &) = delete;

	Resource(Resource &&) = delete;
	Resource &operator=(Resource &&) = delete;

  public:
	std::shared_ptr<RESOURCE> resource;

  protected:
	Descriptor descriptor;
};

template <typename RESOURCE>
Resource<RESOURCE>::Resource(Descriptor dstor) :
    VirtualResource(RsrcLifeTimeType::Transient),
    descriptor(dstor)
{
}

template <typename RESOURCE>
Resource<RESOURCE>::Resource(std::shared_ptr<RESOURCE> rsrc) :
    VirtualResource(RsrcLifeTimeType::Persistent),
    resource(rsrc),
    descriptor{}
{
}






//*********************************Texture Specilization**********************************
template <>
inline void Resource<VkTexture>::Actualize(VkRenderpassManager &renderpass_manager)
{
	if (RsrcLifeTimeType::Persistent != this->life_time)
	{
		resource = renderpass_manager.GetTextureFactory().ProduceUnfilledTexture(*descriptor.tex_img_PP, descriptor.sample_CI, descriptor.img_view_CI);
	}
}


template <>
inline void Resource<VkTexture>::DeActualize(VkRenderpassManager &renderpass_manager)
{
	if (RsrcLifeTimeType::Persistent != this->life_time)
	{
		resource.reset();
	}
}



template <>
inline void Resource<VkTexture>::StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, RsrcUsageInfoInPass &prsrcinfo)
{
	const auto &p_tex_info  = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(prsrcinfo);
	const auto  img_view_CI = p_tex_info.GetImgViewCI();
	const auto  sampler_CI  = p_tex_info.GetSamplerCI();

	if (img_view_CI)
	{
		renderpass_manager.GetTextureFactory().ResetTexImgView(*img_view_CI, *resource);
	}
	if (sampler_CI)
	{
		renderpass_manager.GetTextureFactory().ResetTexSampler(*sampler_CI, *resource);
	}
}

template <>
inline void Resource<VkTexture>::InsertSync(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync)
{
	const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	    .srcAccessMask = source_syn.access_mask,
	    .srcStageMask  = source_syn.stage_mask,
	    .oldLayout     = source_syn.layout_inpass,

	    .dstAccessMask = source_syn.access_mask,
	    .dstStageMask  = source_syn.stage_mask,
	    .newLayout     = source_syn.layout_inpass,
	    //TODO: sub ranges
	    .subresource_range = std::nullopt,
	};
	resource->InsertImageMemoryBarrier(cmd_buf, image_memory_barrier_enhanced);
}


//*********************************Buffer Specilization**********************************
template <>
inline void Resource<VkBufferBase>::Actualize(VkRenderpassManager &renderpass_manager)
{
	if (RsrcLifeTimeType::Persistent != this->life_time)
	{
		resource = renderpass_manager.GetBufferFactory().ProduceBuffer(descriptor.buffer_size, descriptor);
	}
}

template <>
inline void Resource<VkBufferBase>::DeActualize(VkRenderpassManager &renderpass_manager)
{
	if (RsrcLifeTimeType::Persistent != this->life_time)
	{
		resource.reset();
	}
}


template <>
inline void Resource<VkBufferBase>::StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, RsrcUsageInfoInPass &prsrcinfo)
{
	//TODO:
}

template <>
inline void Resource<VkBufferBase>::InsertSync(VkCommandBuffer cmd_buf, const Vk::SyncInfo &source_syn, const Vk::SyncInfo &target_sync)
{
	//TODO:
}

}        // namespace RenderGraph

namespace RenderGraph
{
class PassNode : public DependencyGraph::Node
{
  public:
	virtual void PrePassExecuteRG(DependencyGraph &) = 0;
	virtual void ExecuteRG(VkCommandBuffer)          = 0;

  public:
	PassNode(size_t);

  public:
	~PassNode() override = default;

	PassNode(const PassNode &) = delete;
	PassNode &operator=(const PassNode &) = delete;
	PassNode(PassNode &&)                 = delete;
	PassNode &operator=(PassNode &&) = delete;

  public:
	//这里面所有容器都不能删除元素，否则通过index进行的映射就会出问题，想办法封装一层，不要提供删除的接口
	//**************************************************************
	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, ResourceSlot>> syn_infos;
	//**************************************************************
	std::vector<ResourceSlot> slot_rsrc_init_list;
	std::vector<ResourceSlot> slot_rsrc_recycle_list;
	//**************************************************************
	std::vector<ResourceSlot> slot_rsrc_state_changing_list;
	//**************************************************************
	std::unordered_map<ResourceSlot, VirtualResource::RsrcAccessType> rsrc_accessing_types;
	//**************************************************************

	std::vector<std::pair<ResourceSlot, RsrcUsageInfoSlot>> attachment_rsrcs;
	std::vector<std::pair<ResourceSlot, RsrcUsageInfoSlot>> buffer_rsrcs;
	std::vector<std::pair<ResourceSlot, RsrcUsageInfoSlot>> texture_rsrcs;

  private:
};















class GraphicsPassNode final : public PassNode
{
  public:
	void PrePassExecuteRG(DependencyGraph &RG) override;
	void ExecuteRG(VkCommandBuffer cmd_buf) override;
	GraphicsPassNode(size_t node_handle_);

  public:
	~GraphicsPassNode() override = default;
	GraphicsPassNode()           = delete;

	GraphicsPassNode(const GraphicsPassNode &) = delete;
	GraphicsPassNode &operator=(const GraphicsPassNode &) = delete;
	GraphicsPassNode(GraphicsPassNode &&)                 = delete;
	GraphicsPassNode &operator=(GraphicsPassNode &&) = delete;

	std::shared_ptr<VkRenderpassBaseRG> renderpass{};

  private:
	void GiveAttachmentsToPass(RenderGraph::PassNode &pass, DependencyGraph &RG);
	void GiveTexturesToPass(RenderGraph::PassNode &pass, DependencyGraph &RG);
	void GiveBuffersToPass(RenderGraph::PassNode &pass, DependencyGraph &RG);
};

}        // namespace RenderGraph
