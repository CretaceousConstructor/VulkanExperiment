#pragma once
#include "RenderGraphPass.h"
#include "Vk.h"
#include "VkDeviceManager.h"
#include "VkRenderpassBaseRG.h"
#include "VkRenderpassManager.h"
#include "VkRsrcState.h"
#include "VkRsrcUsageInfoRG.h"

#include <functional>
#include <array>

namespace RenderGraphV0
{
//forward declaration
//class DependencyGraph;
//class PassNode;
//class GraphicsPassNode;
//
//template <typename... Args>
//concept UsageTypeBufConstrain =
//    requires
//{
//	requires(std::is_same<std::unique_ptr<VkBufUsageInfoRG>, Args>() && ...);
//};
//
//template <typename... Args>
//concept UsageTypeTexConstrain =
//    requires
//{
//	requires(std::is_same<std::unique_ptr<VkTexUsageInfoRG>, Args>() && ...);
//};
//
//template <typename... Args>
//concept UsageTypeAttachConstrain =
//    requires
//{
//	requires(std::is_same<std::unique_ptr<VkAttachmentInfo::WithinPassRG>, Args>() && ...);
//};

//
//
//
//
//class PassNode
//{
//  public:
//	PassNode(const std::string name_) :
//	    name(name_)
//	{
//	}
//
//	virtual ~PassNode() = default;
//
//	PassNode(const PassNode &) = delete;
//	PassNode &operator=(const PassNode &) = delete;
//
//	PassNode(PassNode &&) = delete;
//	PassNode &operator=(PassNode &&) = delete;
//
//	virtual void PreExecute()                     = 0;
//	virtual void Execute(VkCommandBuffer cmd_buf) = 0;
//
//  public:
//	//这里面U参数（usage）表示是资源的使用方式，其是传引用进入函数的。
//	//template <typename R, typename U>
//	//class RsrcOutlet
//	//{
//	//  public:
//	//	RsrcOutlet(
//	//	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
//	//	    U &                                                                    usage_) :
//	//	    rsrcmap_itr(std::move(rsrcmap_itr_)), usage(usage_)
//	//	{
//	//	}
//	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
//	//	{
//	//		return rsrcmap_itr;
//	//	}
//
//	//	void AddAccessingPass(PassNode *pass) const
//	//	{
//	//		accessing_passes.push_back(pass);
//	//	}
//	//	std::vector<PassNode *> GetAccessingPasses()
//	//	{
//	//		return accessing_passes;
//	//	}
//	//	U &GetRsrcUsage()
//	//	{
//	//		return usage;
//	//	}
//
//	//  private:
//	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
//	//	U &                                                                    usage;
//	//	std::vector<PassNode *>                                                accessing_passes;
//
//	//	std::vector<PassNode *> accessing_passes_as_in;
//	//	std::vector<PassNode *> accessing_passes_as_out;
//	//};
//
//	//template <typename R, typename U>
//	//class RsrcInlet
//	//{
//	//  public:
//	//	RsrcInlet(
//	//	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
//	//	    U &                                                                    usage_) :
//	//	    rsrcmap_itr(std::move(rsrcmap_itr_)), usage(usage_), providing_passes(nullptr)
//	//	{
//	//	}
//	//	RsrcInlet() = delete;
//
//	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
//	//	{
//	//		return rsrcmap_itr;
//	//	}
//
//	//	void AssignProvidingPass(PassNode *pass)
//	//	{
//	//		providing_passes = pass;
//	//	}
//
//	//	[[nodiscard]] PassNode *GetProvidingPass() const
//	//	{
//	//		return providing_passes;
//	//	}
//
//	//	U &GetRsrcUsage()
//	//	{
//	//		return usage;
//	//	}
//
//	//  private:
//	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
//	//	U &                                                                    usage;
//	//	PassNode *                                                             providing_passes;
//	//};
//
//	template <typename R, typename U>
//	class RsrcInletMultiUsages
//	{
//	  public:
//		RsrcInletMultiUsages(
//		    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
//		    std::vector<U *>                                                       usages_) :
//		    rsrcmap_itr(std::move(rsrcmap_itr_)), usages(std::move(usages_)), providing_passes(nullptr)
//		{
//		}
//		RsrcInletMultiUsages() = delete;
//
//		typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
//		{
//			return rsrcmap_itr;
//		}
//
//		void AssignProvidingPass(PassNode *pass)
//		{
//			providing_passes = pass;
//		}
//
//		PassNode *GetProvidingPass() const
//		{
//			return providing_passes;
//		}
//
//		/*	U &GetRsrcUsage()
//		{
//			return usage;
//		}*/
//
//		std::vector<U *> GetAllRsrcUsages()
//		{
//			return usages;
//		}
//
//	  private:
//		typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
//		std::vector<U *>                                                       usages;
//		PassNode *                                                             providing_passes;
//	};
//
//	//这里面U参数（usage）表示是资源的使用方式，其是传引用进入函数的。
//	template <typename R, typename U>
//	class RsrcOutletMultiUsages
//	{
//	  public:
//		RsrcOutletMultiUsages(
//		    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
//		    std::vector<U *>                                                       usages_) :
//		    rsrcmap_itr(std::move(rsrcmap_itr_)), usages(std::move(usages_))
//		{
//		}
//
//		typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
//		{
//			return rsrcmap_itr;
//		}
//
//		void AddAccessingPass(PassNode *pass)
//		{
//			accessing_passes.push_back(pass);
//		}
//		std::vector<PassNode *> GetAccessingPasses()
//		{
//			return accessing_passes;
//		}
//		//U &GetRsrcUsage()
//		//{
//		//	return usage;
//		//}
//
//		std::vector<U *> GetAllRsrcUsages()
//		{
//			return usages;
//		}
//
//	  private:
//		typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
//		std::vector<U *>                                                       usages;
//		std::vector<PassNode *>                                                accessing_passes;
//
//		//std::vector<PassNode *> accessing_passes_as_in;
//		//std::vector<PassNode *> accessing_passes_as_out;
//	};
//
//  public:
//	std::string name{};
//
//  public:
//	std::unordered_set<const PassNode *> passes_depen_set;
//
//  public:
//	std::vector<std::unique_ptr<VkBufUsageInfoRG>>               buf_usages;
//	std::vector<std::unique_ptr<VkTexUsageInfoRG>>               tex_usages;
//	std::vector<std::unique_ptr<VkAttachmentInfo::WithinPassRG>> attach_usages;
//
//  public:
//	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_init_list;
//	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    tex_init_list;
//	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    attch_init_list;
//
//  public:
//	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_recycle_list;
//	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    tex_recycle_list;
//	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    attch_recycle_list;
//
//  public:
//	//std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>            outs_buf;
//	//std::unordered_map<std::string, RsrcOutlet<VkTexture, VkTexUsageInfoRG>>               outs_tex;
//	//std::unordered_map<std::string, RsrcOutlet<VkTexture, VkAttachmentInfo::WithinPassRG>> outs_attach;
//
//	//std::unordered_map<std::string, RsrcInlet<VkBufferBase, VkBufUsageInfoRG>>            ins_buf;
//	//std::unordered_map<std::string, RsrcInlet<VkTexture, VkTexUsageInfoRG>>               ins_tex;
//	//std::unordered_map<std::string, RsrcInlet<VkTexture, VkAttachmentInfo::WithinPassRG>> ins_attach;
//
//	//===================================================
//	//多使用方式的改进，用在模板类的in out函数中
//	std::unordered_map<std::string, RsrcInletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>            ins_buf_mulu;
//	std::unordered_map<std::string, RsrcInletMultiUsages<VkTexture, VkTexUsageInfoRG>>               ins_tex_mulu;
//	std::unordered_map<std::string, RsrcInletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>> ins_attach_mulu;
//
//	std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>            outs_buf_mulu;
//	std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>               outs_tex_mulu;
//	std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>> outs_attach_mulu;
//
//  public:
//	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator>> buf_syn_infos;
//	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>>    tex_syn_infos;
//	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>>    attch_syn_infos;
//};
//

class GraphEdge
{
  public:
	GraphEdge(PassNode *f, PassNode *t) :
	    from(f), to(t)
	{
	}

	PassNode *from;
	PassNode *to;
};

class GraphNode
{
  public:
	GraphNode(PassNode *node_) :
	    curr_pass(node_)
	{
	}
	void AddEdge(GraphEdge edge)
	{
		edges.push_back(edge);
	}
	PassNode *             curr_pass;
	std::vector<GraphEdge> edges;
};

class DependencyGraph
{
  public:
	DependencyGraph(VkRenderpassManager &renderpass_manager_);

	DependencyGraph()  = delete;
	~DependencyGraph() = default;

	DependencyGraph(const DependencyGraph &) = delete;
	DependencyGraph &operator=(const DependencyGraph &) = delete;
	DependencyGraph(DependencyGraph &&)                 = delete;
	DependencyGraph &operator=(DependencyGraph &&) = delete;

  public:
	VkAttachmentInfo GetAttachmentRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);
	VkAttachmentInfo GetAttachmentRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);

	VkTexUsageInfo GetTextureRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);
	VkTexUsageInfo GetTextureRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);

	VkUniBufUsageInfo GetBufferRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);
	VkUniBufUsageInfo GetBufferRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);

	void StateChangeNoNeedCmdRecordingAttachmentOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);
	void StateChangeNoNeedCmdRecordingAttachmentIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);

	void StateChangeNoNeedCmdRecordingTextureIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);
	void StateChangeNoNeedCmdRecordingTextureOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);

	void StateChangeNoNeedCmdRecordingBufferIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);
	void StateChangeNoNeedCmdRecordingBufferOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index = 0);

	GraphicsPassNode &AddGfxPassNode(std::string name, std::shared_ptr<VkRenderpassBaseRG> renderpass_ptr);

	bool Compile(const VkDeviceManager &device_man);
	bool Execute(VkCommandBuffer cmb);
	bool ExecutRenderGraphV0(VkCommandBuffer cmb);
	bool ParallelExecuteRenderGraphV0(const VkDeviceManager &device_man, VkSemaphore general_rendering_finished_semaphore, VkSemaphore image_available_semaphore);

  private:
	void      GeneratePassNodeDepen();
	PassNode *FindLastAccessingPass(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const;
	void      SortAccessingPasses(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const;
	void      ExecutePass(VkCommandBuffer cmd_buf, PassNode *pass);
	void      RootPassMarking(PassNode *root_pass);
	void      SubGraphMerge();

	bool CompareByFinalExeOrder(const PassNode *a, const PassNode *b, const std::vector<PassNode *> &order);
	void VisitNode(size_t index, PassNode *cur_node_pass, std::vector<bool> &visited, std::vector<bool> &on_stack, std::vector<PassNode *> &topologically_sorted_nodes);

	size_t FindIndexInPassNodeArray(PassNode *node_pass,  const std::vector<std::unique_ptr<GraphicsPassNode>> &all_nodes);

  public:
	//使用unordered map，迭代器除非指向的是被删除的元素，否则迭代器[ 不会失效 ]
	std::unordered_map<std::string, VirtualResource<VkBufferBase>> buffers_map;
	//std::unordered_map<std::string, VirtualResource<VkTexture>>    textures_map;
	//std::unordered_map<std::string, VirtualResource<VkTexture>>    attachments_map;
	std::unordered_map<std::string, VirtualResource<VkTexture>> uni_textures_map;
	std::vector<std::unique_ptr<GraphicsPassNode>>              gfx_pass_nodes;

  private:
	std::vector<PassNode *> final_execution_order_hint;
	//head数组
	//边数组
	std::vector<GraphNode> heads;
	std::vector<GraphEdge> edges;

  private:
	VkRenderpassManager &renderpass_manager;
};

//class GraphicsPassNode : public PassNode
//{
//  public:
//	//virtual void PrePassExecuteRG(DependencyGraph &) = 0;
//	//virtual void ExecuteRG(VkCommandBuffer)          = 0;
//
//  public:
//	GraphicsPassNode(const std::string name_, DependencyGraph &render_graph_) :
//	    PassNode(name_), rg(render_graph_)
//	{
//	}
//
//  public:
//	~GraphicsPassNode() override = default;
//
//	GraphicsPassNode(const GraphicsPassNode &) = delete;
//	GraphicsPassNode &operator=(const GraphicsPassNode &) = delete;
//	GraphicsPassNode(GraphicsPassNode &&)                 = delete;
//	GraphicsPassNode &operator=(GraphicsPassNode &&) = delete;
//
//  public:
//	////这里面所有容器都不能删除元素，否则通过index进行的映射就会出问题，想办法封装一层，不要提供删除的接口
//	////**************************************************************
//	//std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, ResourceSlot>> syn_infos;
//	////**************************************************************
//	//std::vector<ResourceSlot> slot_rsrc_init_list;
//	//std::vector<ResourceSlot> slot_rsrc_recycle_list;
//	////**************************************************************
//	//std::vector<ResourceSlot> slot_rsrc_state_changing_list;
//	////**************************************************************
//	//std::unordered_map<ResourceSlot, VirtualResource::RsrcAccessType> rsrc_accessing_types;
//	////**************************************************************
//
//	//std::vector<std::pair<ResourceSlot, RsrcUsageInfoSlot>> attachment_rsrcs_usages;
//	//std::vector<std::pair<ResourceSlot, RsrcUsageInfoSlot>> buffer_rsrcs_usages;
//	//std::vector<std::pair<ResourceSlot, RsrcUsageInfoSlot>> texture_rsrcs_usages;
//
//	//************************************************************************************************
//	//IN和OUT只代表资源是否会导出给其他pass使用，IN和OUT函数调用时使用的相关的资源 既可能会被读也可能会被写
//
//	//接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
//	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出
//	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
//	//GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
//	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
//	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	//TEMPLATED VERSION for Buffers
//	//接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, Args... all_buf_usages)
//	{
//		//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//		//const auto rsrc_name_outside_RG = rsrc_name + std::string("_") + std::string("Imported");        //外部引入资源，无pass后缀
//		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //外部引入资源
//
//		//首先看是否已经存在于buffers map中，如果没有就首次创建
//		auto rsrc_itr = rg.buffers_map.find(rsrc_name_outside_RG);
//		if (rsrc_itr == rg.buffers_map.end())
//		{
//			const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_buf);        //键值和emplace构造元素所需的参数
//			rsrc_itr            = temp_itr.first;
//		}
//
//		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//创建当前pass的资源的导入口
//		const auto cur_in_itr = ins_buf_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, buf_usages_vec);
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//把usage和资源本身联系起来。
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = rsrc_itr;
//		}
//		//给ItrInRsrcMap增加使用它的pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//资源使用方式存储
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	//接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args... all_buf_usages)
//	{
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//		//从其他pass的outlets中寻找对应资源
//		const auto source_outlet_itr = source_pass.outs_buf_mulu.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//		if (source_pass.outs_buf_mulu.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");
//		}
//
//		//给source outlet增加使用它的pass
//		source_outlet_itr->second.AddAccessingPass(this);
//		//给ItrInRsrcMap 增加使用它的pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		// 构造指向使用方式的指针的数组
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//创建当前pass的资源的导入口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//		const auto cur_in_itr                 = ins_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), buf_usages_vec);
//
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//给inlet赋值提供它的pass
//		cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//		//资源使用方式存储
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//
//		return *this;
//	}
//
//	//buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, Args... all_buf_usages)
//	{
//		//当前资源含有descriptor，资源就是在当前pass创建。
//
//		//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
//		//const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//
//		//资源的命名方式为：资源名 + _
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//		//用传入的lambda函数改变descriptor
//		descriptor_modifier(buf_descriptor);
//
//		//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//		const auto itr = rg.buffers_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, buf_descriptor);
//		//确保用户没有重复添加
//		if (!itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		// 构造指向使用方式的指针的数组
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = itr.first;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		itr.first->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, buf_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exists!");
//		}
//
//		//资源使用方式存储
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[此资源需要在当前pass实体化]
//		buf_init_list.push_back(itr.first);
//
//		return *this;
//	}
//
//	//buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, Args... all_buf_usages)
//	{
//		//const auto rsrc_name_imported_outside = rsrc_name + std::string("_") + std::string("Imported");
//		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//		//首先看imported virtual resource是否已经存在了
//		auto rsrc_itr = rg.buffers_map.find(rsrc_name_imported_outside);
//		//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//		//如果virtual resource不存在，则创建:
//		if (rsrc_itr == rg.buffers_map.end())
//		{
//			const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_buf);        //键值和emplace构造所需的参数
//			rsrc_itr            = temp_itr.first;
//		}
//
//		// 构造指向使用方式的指针的数组
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = rsrc_itr;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto outlet_itr                 = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, buf_usages_vec);
//
//		if (!outlet_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//资源使用方式存储
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	//接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args... all_buf_usages)
//	{
//		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//		//先找一找  源头pass的导出接口是否确实提供了需要的资源
//
//		const auto source_outlet_itr = source_pass.outs_buf_mulu.find(rsrc_name_source_pass_suffix);
//		if (source_pass.outs_buf_mulu.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错，抛出异常
//		}
//
//		//source pass中的outlet添加accessing pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// 构造指向使用方式的指针的数组
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), buf_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//把资源使用方式存储起来
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	//************************************************************************************************
//	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
//	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
//	//GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
//	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
//	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
//
//	//--------------------------------------------------------------------------------------
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args... all_tex_usages)
//	{
//		//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
//		//首先看是否已经存在于buffers map中，如果没有就重新创建
//		auto rsrc_itr = rg.textures_map.find(rsrc_name_outside_RG);
//		if (rsrc_itr == rg.textures_map.end())
//		{
//			const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
//			rsrc_itr            = temp_itr.first;
//		}
//
//		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//创建当前pass的资源的导入口
//		const auto cur_in_itr = ins_tex_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, tex_usages_vec);
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//资源使用方式存储
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args... all_tex_usages)
//	{
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//		//从其他pass的outlets中寻找对应资源
//		const auto source_outlet_itr = source_pass.outs_tex_mulu.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//		if (source_pass.outs_tex_mulu.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");
//		}
//		//给source outlet增加使用它的pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导入口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//		const auto cur_in_itr                 = ins_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), tex_usages_vec);
//
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//给inlet赋值提供它的pass
//		cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//		//资源使用方式存储
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, Args... all_tex_usages)
//	{
//		//当前函数传入参数含有descriptor，资源就是在当前pass创建。
//
//		//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//		//用传入的lambda函数改变descriptor
//		descriptor_modifier(tex_descriptor);
//
//		//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//		const auto itr = rg.textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//		//确保用户没有重复添加
//		if (!itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = itr.first;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		itr.first->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, tex_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//资源使用方式存储
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[此资源需要在当前pass实体化]
//		tex_init_list.push_back(itr.first);
//
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args... all_tex_usages)
//	{
//		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//		//首先看imported virtual resource是否已经存在了
//		auto rsrc_itr = rg.textures_map.find(rsrc_name_imported_outside);
//		//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//		//如果virtual resource不存在，则创建:
//		if (rsrc_itr == rg.textures_map.end())
//		{
//			const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
//			rsrc_itr            = temp_itr.first;
//		}
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto outlet_itr                 = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, tex_usages_vec);
//
//		if (!outlet_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//资源使用方式存储
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args... all_tex_usages)
//	{
//		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//		//先找一找源头pass的导出接口是否确实提供了需要的资源
//		const auto source_outlet_itr = source_pass.outs_tex_mulu.find(rsrc_name_source_pass_suffix);
//		if (source_pass.outs_tex_mulu.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错
//		}
//
//		//source pass中的outlet添加accessing pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), tex_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//资源使用方式存储
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	//************************************************************************************************
//	/*GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
//
//	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
//
//	GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
//	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
//	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);*/
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args... all_attach_usages)
//	{
//		//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
//		//首先看是否已经存在于buffers map中，如果没有就重新创建
//		auto rsrc_itr = rg.attachments_map.find(rsrc_name_outside_RG);
//		if (rsrc_itr == rg.attachments_map.end())
//		{
//			const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
//			rsrc_itr            = temp_itr.first;
//		}
//
//		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//创建当前pass的资源的导入口
//		const auto cur_in_itr = ins_attach_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, attach_usages_vec);
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//资源使用方式存储
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args... all_attach_usages)
//	{
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//		//从其他pass的outlets中寻找对应资源
//		const auto source_outlet_itr = source_pass.outs_attach_mulu.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//		if (source_pass.outs_attach_mulu.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");
//		}
//		//给source outlet增加使用它的pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导入口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//		const auto cur_in_itr                 = ins_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), attach_usages_vec);
//
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//给inlet赋值提供它的pass
//		cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//		//资源使用方式存储
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, Args... all_attach_usages)
//	{
//		//当前资源含有descriptor，资源就是在当前pass创建。
//
//		//资源的命名方式为：资源名 + _
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//		//用传入的lambda函数改变descriptor
//		descriptor_modifier(tex_descriptor);
//
//		//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//		const auto itr = rg.attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//		//确保用户没有重复添加
//		if (!itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = itr.first;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		itr.first->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, attach_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//资源使用方式存储
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[此资源需要在当前pass实体化]
//		attch_init_list.push_back(itr.first);
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args... all_attach_usages)
//	{
//		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//		//首先看imported virtual resource是否已经存在了
//		auto rsrc_itr = rg.attachments_map.find(rsrc_name_imported_outside);
//		//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//		//如果virtual resource不存在，则创建:
//		if (rsrc_itr == rg.attachments_map.end())
//		{
//			const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
//			rsrc_itr            = temp_itr.first;
//		}
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//给ItrInRsrcMap增加使用它的pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto outlet_itr                 = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, attach_usages_vec);
//
//		if (!outlet_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//资源使用方式存储
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args... all_attach_usages)
//	{
//		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//		//先找一找源头pass的导出接口是否确实提供了需要的资源
//
//		const auto source_outlet_itr = source_pass.outs_attach_mulu.find(rsrc_name_source_pass_suffix);
//		if (source_pass.outs_attach_mulu.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错
//		}
//
//		//source pass中的outlet添加accessing pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// 构造指向使用方式指针的数组（因为传进来的全都是unique ptr，所以不能直接移交所有权
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//把资源使用方式和虚拟资源联系起来
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//
//
//		//给ItrInRsrcMap增加使用它的pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//创建当前pass的资源的导出口
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), attach_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//资源使用方式存储
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[此资源不需要在当前pass实体化]
//		return *this;
//	}
//
//
//
//
//
//	//************************************************************************************************
//
//  public:
//	void PreExecute() override;
//	void Execute(VkCommandBuffer cmd_buf) override;
//
//  private:
//	DependencyGraph &                   rg;
//	std::shared_ptr<VkRenderpassBaseRG> renderpass{};
//};

}        // namespace RenderGraphV0
