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
//	//������U������usage����ʾ����Դ��ʹ�÷�ʽ�����Ǵ����ý��뺯���ġ�
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
//	//������U������usage����ʾ����Դ��ʹ�÷�ʽ�����Ǵ����ý��뺯���ġ�
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
//	//��ʹ�÷�ʽ�ĸĽ�������ģ�����in out������
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
	//ʹ��unordered map������������ָ����Ǳ�ɾ����Ԫ�أ����������[ ����ʧЧ ]
	std::unordered_map<std::string, VirtualResource<VkBufferBase>> buffers_map;
	//std::unordered_map<std::string, VirtualResource<VkTexture>>    textures_map;
	//std::unordered_map<std::string, VirtualResource<VkTexture>>    attachments_map;
	std::unordered_map<std::string, VirtualResource<VkTexture>> uni_textures_map;
	std::vector<std::unique_ptr<GraphicsPassNode>>              gfx_pass_nodes;

  private:
	std::vector<PassNode *> final_execution_order_hint;
	//head����
	//������
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
//	////��������������������ɾ��Ԫ�أ�����ͨ��index���е�ӳ��ͻ�����⣬��취��װһ�㣬��Ҫ�ṩɾ���Ľӿ�
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
//	//IN��OUTֻ������Դ�Ƿ�ᵼ��������passʹ�ã�IN��OUT��������ʱʹ�õ���ص���Դ �ȿ��ܻᱻ��Ҳ���ܻᱻд
//
//	//����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��
//	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
//	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
//	//GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
//	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	////����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
//	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);
//
//	//TEMPLATED VERSION for Buffers
//	//����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, Args... all_buf_usages)
//	{
//		//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//		//const auto rsrc_name_outside_RG = rsrc_name + std::string("_") + std::string("Imported");        //�ⲿ������Դ����pass��׺
//		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //�ⲿ������Դ
//
//		//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о��״δ���
//		auto rsrc_itr = rg.buffers_map.find(rsrc_name_outside_RG);
//		if (rsrc_itr == rg.buffers_map.end())
//		{
//			const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_buf);        //��ֵ��emplace����Ԫ������Ĳ���
//			rsrc_itr            = temp_itr.first;
//		}
//
//		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //��pass��������Ϊ��׺
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//������ǰpass����Դ�ĵ����
//		const auto cur_in_itr = ins_buf_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, buf_usages_vec);
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��usage����Դ������ϵ������
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = rsrc_itr;
//		}
//		//��ItrInRsrcMap����ʹ������pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//��Դʹ�÷�ʽ�洢
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	//����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args... all_buf_usages)
//	{
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //��GbufferPos_GPass_Out
//
//		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
//		const auto source_outlet_itr = source_pass.outs_buf_mulu.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
//		if (source_pass.outs_buf_mulu.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");
//		}
//
//		//��source outlet����ʹ������pass
//		source_outlet_itr->second.AddAccessingPass(this);
//		//��ItrInRsrcMap ����ʹ������pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		// ����ָ��ʹ�÷�ʽ��ָ�������
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//������ǰpass����Դ�ĵ����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//		const auto cur_in_itr                 = ins_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), buf_usages_vec);
//
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��inlet��ֵ�ṩ����pass
//		cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//		//��Դʹ�÷�ʽ�洢
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//
//		return *this;
//	}
//
//	//buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, Args... all_buf_usages)
//	{
//		//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������
//
//		//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
//		//const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//
//		//��Դ��������ʽΪ����Դ�� + _
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//		//�ô����lambda�����ı�descriptor
//		descriptor_modifier(buf_descriptor);
//
//		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
//		const auto itr = rg.buffers_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, buf_descriptor);
//		//ȷ���û�û���ظ����
//		if (!itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		// ����ָ��ʹ�÷�ʽ��ָ�������
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = itr.first;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		itr.first->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, buf_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exists!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
//		buf_init_list.push_back(itr.first);
//
//		return *this;
//	}
//
//	//buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, Args... all_buf_usages)
//	{
//		//const auto rsrc_name_imported_outside = rsrc_name + std::string("_") + std::string("Imported");
//		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
//		auto rsrc_itr = rg.buffers_map.find(rsrc_name_imported_outside);
//		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��
//
//		//���virtual resource�����ڣ��򴴽�:
//		if (rsrc_itr == rg.buffers_map.end())
//		{
//			const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_buf);        //��ֵ��emplace��������Ĳ���
//			rsrc_itr            = temp_itr.first;
//		}
//
//		// ����ָ��ʹ�÷�ʽ��ָ�������
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = rsrc_itr;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto outlet_itr                 = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, buf_usages_vec);
//
//		if (!outlet_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	//����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
//	template <typename... Args>
//	requires UsageTypeBufConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args... all_buf_usages)
//	{
//		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//		//����һ��  Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
//
//		const auto source_outlet_itr = source_pass.outs_buf_mulu.find(rsrc_name_source_pass_suffix);
//		if (source_pass.outs_buf_mulu.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ����׳��쳣
//		}
//
//		//source pass�е�outlet���accessing pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// ����ָ��ʹ�÷�ʽ��ָ�������
//		std::vector<VkBufUsageInfoRG *> buf_usages_vec{(&(*all_buf_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_buf_usage : buf_usages_vec)
//		{
//			ptr_buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), buf_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//����Դʹ�÷�ʽ�洢����
//		(this->buf_usages.push_back(std::move(all_buf_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
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
//		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
//		//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о����´���
//		auto rsrc_itr = rg.textures_map.find(rsrc_name_outside_RG);
//		if (rsrc_itr == rg.textures_map.end())
//		{
//			const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
//			rsrc_itr            = temp_itr.first;
//		}
//
//		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //��pass��������Ϊ��׺
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//������ǰpass����Դ�ĵ����
//		const auto cur_in_itr = ins_tex_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, tex_usages_vec);
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//��Դʹ�÷�ʽ�洢
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args... all_tex_usages)
//	{
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //��GbufferPos_GPass_Out
//
//		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
//		const auto source_outlet_itr = source_pass.outs_tex_mulu.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
//		if (source_pass.outs_tex_mulu.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");
//		}
//		//��source outlet����ʹ������pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//		const auto cur_in_itr                 = ins_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), tex_usages_vec);
//
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��inlet��ֵ�ṩ����pass
//		cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//		//��Դʹ�÷�ʽ�洢
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, Args... all_tex_usages)
//	{
//		//��ǰ���������������descriptor����Դ�����ڵ�ǰpass������
//
//		//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//		//�ô����lambda�����ı�descriptor
//		descriptor_modifier(tex_descriptor);
//
//		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
//		const auto itr = rg.textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//		//ȷ���û�û���ظ����
//		if (!itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = itr.first;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		itr.first->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, tex_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
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
//		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
//		auto rsrc_itr = rg.textures_map.find(rsrc_name_imported_outside);
//		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��
//
//		//���virtual resource�����ڣ��򴴽�:
//		if (rsrc_itr == rg.textures_map.end())
//		{
//			const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
//			rsrc_itr            = temp_itr.first;
//		}
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto outlet_itr                 = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, tex_usages_vec);
//
//		if (!outlet_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeTexConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args... all_tex_usages)
//	{
//		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//		//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
//		const auto source_outlet_itr = source_pass.outs_tex_mulu.find(rsrc_name_source_pass_suffix);
//		if (source_pass.outs_tex_mulu.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ���
//		}
//
//		//source pass�е�outlet���accessing pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkTexUsageInfoRG *> tex_usages_vec{(&(*all_tex_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : tex_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), tex_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->tex_usages.push_back(std::move(all_tex_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
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
//		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
//		//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о����´���
//		auto rsrc_itr = rg.attachments_map.find(rsrc_name_outside_RG);
//		if (rsrc_itr == rg.attachments_map.end())
//		{
//			const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
//			rsrc_itr            = temp_itr.first;
//		}
//
//		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //��pass��������Ϊ��׺
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//������ǰpass����Դ�ĵ����
//		const auto cur_in_itr = ins_attach_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, attach_usages_vec);
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//��Դʹ�÷�ʽ�洢
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args... all_attach_usages)
//	{
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //��GbufferPos_GPass_Out
//
//		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
//		const auto source_outlet_itr = source_pass.outs_attach_mulu.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
//		if (source_pass.outs_attach_mulu.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");
//		}
//		//��source outlet����ʹ������pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//		const auto cur_in_itr                 = ins_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), attach_usages_vec);
//
//		if (!cur_in_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��inlet��ֵ�ṩ����pass
//		cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//		//��Դʹ�÷�ʽ�洢
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, Args... all_attach_usages)
//	{
//		//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������
//
//		//��Դ��������ʽΪ����Դ�� + _
//		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//		//�ô����lambda�����ı�descriptor
//		descriptor_modifier(tex_descriptor);
//
//		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
//		const auto itr = rg.attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//		//ȷ���û�û���ظ����
//		if (!itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = itr.first;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		itr.first->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, attach_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
//		attch_init_list.push_back(itr.first);
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args... all_attach_usages)
//	{
//		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
//		auto rsrc_itr = rg.attachments_map.find(rsrc_name_imported_outside);
//		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��
//
//		//���virtual resource�����ڣ��򴴽�:
//		if (rsrc_itr == rg.attachments_map.end())
//		{
//			const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
//			rsrc_itr            = temp_itr.first;
//		}
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = rsrc_itr;
//		}
//
//		//��ItrInRsrcMap����ʹ������pass
//		rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto outlet_itr                 = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, attach_usages_vec);
//
//		if (!outlet_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//		return *this;
//	}
//
//	template <typename... Args>
//	requires UsageTypeAttachConstrain<Args...>
//	    GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args... all_attach_usages)
//	{
//		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//		//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
//
//		const auto source_outlet_itr = source_pass.outs_attach_mulu.find(rsrc_name_source_pass_suffix);
//		if (source_pass.outs_attach_mulu.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//		{
//			throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ���
//		}
//
//		//source pass�е�outlet���accessing pass
//		source_outlet_itr->second.AddAccessingPass(this);
//
//		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
//		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{(&(*all_attach_usages))...};
//
//		//����Դʹ�÷�ʽ��������Դ��ϵ����
//		for (const auto ptr_tex_usage : attach_usages_vec)
//		{
//			ptr_tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//		}
//
//
//
//		//��ItrInRsrcMap����ʹ������pass
//		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//		//������ǰpass����Դ�ĵ�����
//		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//		const auto cur_out_itr                = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), attach_usages_vec);
//
//		if (!cur_out_itr.second)
//		{
//			throw std::runtime_error("resource with the same name already exist!");
//		}
//
//		//��Դʹ�÷�ʽ�洢
//		(this->attach_usages.push_back(std::move(all_attach_usages)), ...);
//
//		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
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
