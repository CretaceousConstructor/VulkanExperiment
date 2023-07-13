#include "RenderGraph.h"

RenderGraph::DependencyGraph::Edge::Edge(const PassNodeHandle f, const PassNodeHandle t) :
    from(f), to(t)
{
}

void RenderGraph::DependencyGraph::Node::AddOutGoingEdge(Edge e)
{
	outgoing_edges.push_back(e);
}

void RenderGraph::DependencyGraph::Node::AddInComingEdge(Edge e)
{
	incoming_edges.push_back(e);
}

RenderGraph::DependencyGraph::Node::Node(size_t node_handle_) :
    node_handle(node_handle_)
{
}

RenderGraph::DependencyGraph::ResourceNode::ResourceNode() :
    DependencyGraph::Node(0)
{
}

RenderGraph::DependencyGraph::DependencyGraph(VkRenderpassManager &renderpass_manager_) :
    renderpass_manager(renderpass_manager_)
{
}

RenderGraph::DependencyGraph::PassNodeHandle RenderGraph::DependencyGraph::AddGfxPassNode(std::shared_ptr<VkRenderpassBase> renderpass, const std::string name, const std::vector<ResourceSlot> inputs, const std::vector<ResourceSlot> putputs)
{
	//TODO:这里对inpus和putputs进行安全性检查，如果一个资源要被读2次以上或者写2次以上，则报错
	//TODO:处理同一个pass读后写，写后读的问题。用两个inputs，两个putputs？
	//CREATE PASSNODE
	auto p_pass_node              = std::make_unique<GraphicsPassNode>(nodes_of_passes.size());
	p_pass_node->node_handle      = nodes_of_passes.size();
	p_pass_node->renderpass       = std::move(renderpass);
	const auto &current_pass_node = nodes_of_passes.emplace_back(std::move(p_pass_node));

	//CREATE INCOMING EDGES TO CURRENT PASS NODE
	//accessing type decided resources
	std::unordered_set<ResourceSlot> accessing_type_decided_resources;
	for (const auto &input_slot : inputs)
	{
		ResourceNode &rsrc_node = GetResourceNode(input_slot);
		rsrc_node.node_handle   = input_slot.node_handle;

		const auto p_edge = Edge(rsrc_node.node_handle, current_pass_node->node_handle);

		rsrc_node.ref_count += 1;
		rsrc_node.AddOutGoingEdge(p_edge);
		current_pass_node->AddInComingEdge(p_edge);
		current_pass_node->rsrc_accessing_types.insert({input_slot, VirtualResource::RsrcAccessType::Read});
	}

	//CREATE OUTGOING EDGES TO CURRENT PASS NODE
	for (const auto &output_slot : putputs)
	{
		ResourceNode &rsrc_node = GetResourceNode(output_slot);
		rsrc_node.node_handle   = output_slot.node_handle;

		const auto p_edge = Edge(current_pass_node->node_handle, rsrc_node.node_handle);

		rsrc_node.ref_count += 1;
		rsrc_node.AddInComingEdge(p_edge);
		current_pass_node->AddOutGoingEdge(p_edge);
		current_pass_node->rsrc_accessing_types.insert({output_slot, VirtualResource::RsrcAccessType::Write});
	}

	return (nodes_of_passes.size() - 1);
}

void RenderGraph::DependencyGraph::RegisterRsrcsUsage(const PassNodeHandle pass_node_handle, const ResourceSlot rsrc_slot, std::unique_ptr<Vk::RsrcUsageInfoInPass> rsrc_info)
{
	const auto info_t = rsrc_info->GetInfoType();

	rsrc_usage_infos.push_back(std::move(rsrc_info));
	const RsrcUsageHandle rsrc_info_handle = rsrc_usage_infos.size() - 1;
	auto &                pass             = *nodes_of_passes[pass_node_handle];



	//把使用信息分发到不同的容器中
	if (Vk::RsrcInfoType::Attachment == info_t)
	{
		pass.attachment_rsrcs.emplace_back(rsrc_slot, RsrcUsageInfoSlot{rsrc_info_handle});
	}

	if (Vk::RsrcInfoType::UniformBuffer == info_t)
	{
		pass.uniform_buffer_rsrcs.emplace_back(rsrc_slot, RsrcUsageInfoSlot{rsrc_info_handle});
	}


	if (Vk::RsrcInfoType::Texture == info_t)
	{
		pass.texture_rsrcs.emplace_back(rsrc_slot, RsrcUsageInfoSlot{rsrc_info_handle});
	}


	//同时在资源使用记录表中 记录此资源的使用
	if (resources_usage_recording_table.contains(rsrc_slot))
	{
		resources_usage_recording_table[rsrc_slot].back() = std::pair{pass_node_handle, RsrcUsageInfoSlot{rsrc_info_handle}};
	}
	else
	{
		resources_usage_recording_table[rsrc_slot].front() = std::pair{pass_node_handle, RsrcUsageInfoSlot{rsrc_info_handle}};
	}



}

void RenderGraph::DependencyGraph::GeneratePassNodeDepen(std::vector<std::unordered_set<size_t>> &pass_node_dependencies) const
{
	for (size_t i = 0; i < nodes_of_passes.size(); ++i)
	{
		const auto &p_pass_node = nodes_of_passes[i];

		auto &current_pass_node_depen = pass_node_dependencies[i];
		for (const auto p_e : p_pass_node->incoming_edges)
		{
			const auto &rsrc_node = nodes_of_resources[p_e.from];
			if (!rsrc_node->pass_accessing_order.empty())
			{
				//TODO: you konw the drill
			}

			for (const auto &p_e_of_rsrc_node : rsrc_node->incoming_edges)
			{
				current_pass_node_depen.insert(p_e_of_rsrc_node.from);
			}
		}
	}
}

bool RenderGraph::DependencyGraph::Move()
{
	return true;
}

bool RenderGraph::DependencyGraph::Compile()
{
	//TODO: Test against invalid passes and unused resources
	final_execution_order.clear();
	std::vector<const PassNode *>           pass_nodes_excluded;
	std::vector<const PassNode *>           temp_final_execution_order;
	std::vector<std::unordered_set<size_t>> pass_node_dependencies(nodes_of_passes.size());
	GeneratePassNodeDepen(pass_node_dependencies);

	//Topological Sorting
	//step 1:找到入度为0的pass节点，如果有多个，则从编号小的开始找；
	//step 2:将该节点的编号输出；
	//step 3:将该节点删除，同时将所有由该节点出发的有向边删除；
	//step 4:循环进行 2 和 3 ，直到图中的图中所有点的入度都为零；
	//step 5:拓扑排序结束；

	while (true)
	{
		for (size_t i = 0; i < nodes_of_passes.size(); ++i)
		{
			const auto &p_pass_node = *nodes_of_passes[i];

			if (pass_nodes_excluded.end() == std::find(pass_nodes_excluded.begin(), pass_nodes_excluded.end(), &p_pass_node))        //当前pass node还没有被移除到执行队列
			{
				if (pass_node_dependencies[i].empty())        //入度为0的pass节点,运行下面的：
				{
					final_execution_order.push_back(i);
					temp_final_execution_order.push_back(&p_pass_node);
					pass_nodes_excluded.push_back(&p_pass_node);
				}
				else
				{
					bool no_depen = true;
					for (const auto depen : pass_node_dependencies[i])
					{
						const auto &temp_pass_node = *nodes_of_passes[depen];
						if (pass_nodes_excluded.end() == std::find(pass_nodes_excluded.begin(), pass_nodes_excluded.end(), &temp_pass_node))
						{
							no_depen = false;
							break;
						}
					}

					if (no_depen)
					{
						final_execution_order.push_back(i);
						temp_final_execution_order.push_back(&p_pass_node);
						pass_nodes_excluded.push_back(&p_pass_node);
					}
				}
			}
		}
		if (final_execution_order.size() == pass_nodes_excluded.size())
		{
			break;
		}
	}

	//MAKE SURE that passes within resources_usage_recording_table are in correct order
	for (auto &[_, rsrc_usage_info_of_two_passes] : resources_usage_recording_table)
	{
		if (rsrc_usage_info_of_two_passes.front().first && rsrc_usage_info_of_two_passes.back().first)
		{
			if (!IfPassInOrder(rsrc_usage_info_of_two_passes.front().first.value(), rsrc_usage_info_of_two_passes.back().first.value()))
			{
				std::swap(rsrc_usage_info_of_two_passes.front(), rsrc_usage_info_of_two_passes.back());
			}
		}
	}

	//INFERENCE of the loading operation of ATTACHMENT
	for (const auto &pass : nodes_of_passes)        //for all the passes
	{
		for (auto &attach : pass->attachment_rsrcs)        //take all the attachment rsrcs of the pass
		{
			const auto &rsrc_node   = *nodes_of_resources[attach.first.resource_handle];
			auto &      attach_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*rsrc_usage_infos[attach.second.handle]);


			//类型std::array<std::pair<std::optional<PassNodeHandle>, RsrcUsageInfoSlot>, 2>
			const auto &rsrc_usage_info_of_two_passes = resources_usage_recording_table[attach.first];

			if (rsrc_node.pass_accessing_order.empty())        //no Resource Reuse的情况
			{
				if (pass == nodes_of_passes[rsrc_usage_info_of_two_passes.front().first.value()])        //当前pass是attachment被使用的第一个pass
				{
					const auto accessing_type = nodes_of_passes[rsrc_usage_info_of_two_passes.front().first.value()]->rsrc_accessing_types[attach.first];
					if (VirtualResource::RsrcAccessType::Write == accessing_type)
					{
						attach_info.load_op  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
						attach_info.store_op = VK_ATTACHMENT_STORE_OP_STORE;
					}
				}
				else                                                                                     //当前pass是attachment被使用的第二个pass
				{
					const auto accessing_type = nodes_of_passes[rsrc_usage_info_of_two_passes.back().first.value()]->rsrc_accessing_types[attach.first];
					if (VirtualResource::RsrcAccessType::Read == accessing_type)
					{
						attach_info.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
						attach_info.store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					}
				}
			}
			else
			{
				//TODO:
			}
		}
	}

	return true;
}

bool RenderGraph::DependencyGraph::Execute(VkCommandBuffer cmb)
{
	using namespace std;

	for (auto rsrc_slot : slots_of_rsrcs)
	{
		const array<pair<optional<PassNodeHandle>, RsrcUsageInfoSlot>, 2> &pass_and_usage_idx_vec = resources_usage_recording_table[rsrc_slot];

		if (!pass_and_usage_idx_vec.front().first.has_value())
		{
			throw runtime_error("error!");
		}
		auto &first_pass = *nodes_of_passes[pass_and_usage_idx_vec.front().first.value()];

		//*****************************************************************************************
		if (nodes_of_resources[rsrc_slot.node_handle]->pass_accessing_order.empty())
		{
			//DECIDING WHICH RESOURCES NEED TO BE INITIALIZED WITHIN THE PASS
			if (VirtualResource::RsrcLifeTimeType::Persistent != resources[rsrc_slot.resource_handle]->life_time)        // persistent resources are from outside RG,don't need to be init-ed
			{
				first_pass.slot_rsrc_init_list.push_back(rsrc_slot);
			}

			//DECIDING WHEN RESOURCES CAN BE DESTROYED
			if (pass_and_usage_idx_vec.back().first)
			{
				auto &second_pass = *nodes_of_passes[pass_and_usage_idx_vec.back().first.value()];
				second_pass.slot_rsrc_recycle_list.push_back(rsrc_slot);
			}
			else
			{
				first_pass.slot_rsrc_recycle_list.push_back(rsrc_slot);
			}

			//RECORDING WHICH RESOURCES NEED STATES CHANGES
			first_pass.slot_rsrc_state_changing.push_back(rsrc_slot);
			if (pass_and_usage_idx_vec.back().first)
			{
				auto &second_pass = *nodes_of_passes[pass_and_usage_idx_vec.back().first.value()];
				second_pass.slot_rsrc_state_changing.push_back(rsrc_slot);
			}
			else
			{
			}
		}
		else
		{
		}
		//*****************************************************************************************

		//RECORDING THE SYNC INFORMATION
		Vk::SyncInfo syn_info_source = rsrc_usage_infos[pass_and_usage_idx_vec.front().second.handle]->GetSynInfo();
		Vk::SyncInfo syn_info_target{
		    .access_mask   = VK_ACCESS_NONE,
		    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};
		if (pass_and_usage_idx_vec.back().first)
		{
			auto &second_pass = *nodes_of_passes[pass_and_usage_idx_vec.back().first.value()];
			syn_info_target   = rsrc_usage_infos[pass_and_usage_idx_vec.back().second.handle]->GetSynInfo();
			second_pass.syn_infos.emplace_back(std::tuple{syn_info_source, syn_info_target, rsrc_slot});
		}
		else
		{
			std::swap(syn_info_source, syn_info_target);
			first_pass.syn_infos.emplace_back(std::tuple{syn_info_source, syn_info_target, rsrc_slot});
		}
	}

	//PASSES EXECUTION(TODO:ORGANIZE PASSES INTO GROUPS FOR PARALLEL EXECUTION)
	for (const auto pass_node_handle : final_execution_order)
	{
		ExecutePass(cmb, pass_node_handle);
	}
	return true;
}

void RenderGraph::DependencyGraph::ExecutePass(VkCommandBuffer cmd_buf, const PassNodeHandle pass_node_handle)
{
	auto &pass = *nodes_of_passes[pass_node_handle];

	//REQUIRED RESOURCES INITIALIZATION
	for (const auto &slot : pass.slot_rsrc_init_list)
	{
		VirtualResource *pvr = resources[slot.resource_handle];
		pvr->Actualize(renderpass_manager);
	}

	//CHANGE STATES OF ALL RESOURCES THAT NEED TO HAVE NEW STATE WITHIN THE CURRENT PASS
	for (const auto rsrc_slot : pass.slot_rsrc_state_changing)
	{
		const auto &pass_node_handle_and_usage_handle_vec = resources_usage_recording_table[rsrc_slot];

		if (pass_node_handle == pass_node_handle_and_usage_handle_vec.front().first.value())
		{
			VirtualResource *pvr = resources[rsrc_slot.resource_handle];
			pvr->ChangeState(renderpass_manager, *rsrc_usage_infos[pass_node_handle_and_usage_handle_vec.front().second.handle]);
		}

		if (pass_node_handle_and_usage_handle_vec.back().first && (pass_node_handle == pass_node_handle_and_usage_handle_vec.back().first.value()))
		{
			VirtualResource *pvr = resources[rsrc_slot.resource_handle];
			pvr->ChangeState(renderpass_manager, *rsrc_usage_infos[pass_node_handle_and_usage_handle_vec.back().second.handle]);
		}
	}

	//BARRIERS INSERSION Pre-Pass
	for (auto &[source_syn, target_syn, resource_slot] : pass.syn_infos)
	{
		VirtualResource *pvr = resources[resource_slot.resource_handle];
		pvr->InsertSync(cmd_buf, source_syn, target_syn);
	}

	//RENDERPASS INITIALIZATION AND EXECUTION
	pass.PrePassExecuteRG(*this);
	pass.ExecuteRG(cmd_buf);

	//TODO: BARRIERS INSERSION After-Pass
}

bool RenderGraph::DependencyGraph::IfPassInOrder(const PassNodeHandle fir, const PassNodeHandle sec) const
{
	const auto it_f = std::find(final_execution_order.begin(), final_execution_order.end(), fir);
	const auto it_s = std::find(final_execution_order.begin(), final_execution_order.end(), sec);

	return (it_f < it_s);
}

RenderGraph::VirtualResource &RenderGraph::DependencyGraph::GetResource(const ResourceSlot &slot)
{
	return *resources[slot.resource_handle];
}

RenderGraph::DependencyGraph::ResourceNode &RenderGraph::DependencyGraph::GetResourceNode(const ResourceSlot &slot)
{
	return *nodes_of_resources[slot.node_handle];
}

RenderGraph::VirtualResource::VirtualResource(RsrcLifeTimeType life_time_) :
    life_time(life_time_)
{
}

RenderGraph::PassNode::PassNode(size_t node_id_) :
    Node(node_id_)
{
}

RenderGraph::GraphicsPassNode::GraphicsPassNode(size_t node_handle_) :
    PassNode(node_handle_)
{
}

void RenderGraph::GraphicsPassNode::PrePassExecuteRG(DependencyGraph &RG)
{
	CreateAttachmentsRG(*this, RG);
	CreateUniformBufferDescriptorsRG(*this, RG);

	ResourceInitRG(*this, RG);

	CreateLocalCommandBuffersRG();

	CreateDescriptorSetPoolsRG();
	CreateDescriptorSetLayoutRG();
	CreateDescriptorSetsRG();

	CreateGraphicsPipelineLayoutRG();
	CreateShadersRG();
	CreateGraphicsPipelineRG();
}

void RenderGraph::GraphicsPassNode::ExecuteRG(VkCommandBuffer cmd_buf)
{
	renderpass->BeginRenderpassRG(cmd_buf);
	renderpass->UpdateDescriptorSetsRG();
	renderpass->RecordRenderpassCommandRG(cmd_buf);
	renderpass->EndRenderpassRG(cmd_buf);
}

void RenderGraph::GraphicsPassNode::ResourceInitRG(RenderGraph::PassNode &pass, DependencyGraph &RG)
{
	renderpass->ResourceInitRG();
}

void RenderGraph::GraphicsPassNode::CreateLocalCommandBuffersRG()
{
	renderpass->CreateLocalCommandBuffersRG();
}

void RenderGraph::GraphicsPassNode::CreateDescriptorSetPoolsRG()
{
	renderpass->CreateDescriptorSetPoolsRG();
}

void RenderGraph::GraphicsPassNode::CreateDescriptorSetLayoutRG()
{
	renderpass->CreateDescriptorSetLayoutRG();
}

void RenderGraph::GraphicsPassNode::CreateDescriptorSetsRG()
{
	renderpass->CreateDescriptorSetsRG();
}

void RenderGraph::GraphicsPassNode::CreateAttachmentsRG(RenderGraph::PassNode &pass, DependencyGraph &RG)
{
	std::vector<VkAttachmentInfo> attachment_infos;

	std::sort(pass.attachment_rsrcs.begin(), pass.attachment_rsrcs.end(),
	          [&RG](const std::pair<ResourceSlot, RsrcUsageInfoSlot> &lhs, const std::pair<ResourceSlot, RsrcUsageInfoSlot> &rhs) -> bool {
		          //const auto lhs_attch_info = std::dynamic_pointer_cast<VkAttachmentInfo::WithinPassRG>(RG.rsrc_usage_infos[lhs.second.handle]);

		          const auto &lhs_attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*RG.rsrc_usage_infos[lhs.second.handle]);
		          const auto &rhs_attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*RG.rsrc_usage_infos[rhs.second.handle]);
		          return lhs_attch_info.attachment_index > rhs_attch_info.attachment_index;
	          });

	for (const auto &attach_rsrc : pass.attachment_rsrcs)
	{
		const auto &p_attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*RG.rsrc_usage_infos[attach_rsrc.second.handle]);

		const VkAttachmentInfo::WithinPass G_albedo_meme{p_attch_info};

		const auto rsrc_tex = dynamic_cast<Resource<VkTexture> *>(RG.resources[attach_rsrc.first.resource_handle]);

		//const VkAttachmentInfo::WithinPass G_albedo_meme{
		//    .format           = DeferedRendering::G_albedo_format,
		//    .attachment_index = Vk::AttachmentIndex<2>,

		//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
		//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

		//    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
		//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

		//    .type        = VkAttachmentInfo::Type::ColorAttachment,
		//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
		//};

		attachment_infos.emplace_back(VkAttachmentInfo{G_albedo_meme, rsrc_tex->resource});
	}
	renderpass->GetAttachmentsRG(std::move(attachment_infos));
}

void RenderGraph::GraphicsPassNode::CreateUniformBufferDescriptorsRG(RenderGraph::PassNode &pass, DependencyGraph &RG)
{
	std::vector<VkUniBufUsageInfo> uniform_buffer_infos;

	std::sort(pass.uniform_buffer_rsrcs.begin(), pass.uniform_buffer_rsrcs.end(),
	          [&RG](const std::pair<ResourceSlot, RsrcUsageInfoSlot> &lhs, const std::pair<ResourceSlot, RsrcUsageInfoSlot> &rhs) -> bool {
		          const auto &lhs_attch_info = dynamic_cast<VkUniBufUsageInfo &>(*RG.rsrc_usage_infos[lhs.second.handle]);
		          const auto &rhs_attch_info = dynamic_cast<VkUniBufUsageInfo &>(*RG.rsrc_usage_infos[rhs.second.handle]);

		          if (lhs_attch_info.dst_binding == rhs_attch_info.dst_binding)
		          {
			          return lhs_attch_info.dst_array_element > rhs_attch_info.dst_array_element;
		          }

		          return lhs_attch_info.dst_binding > rhs_attch_info.dst_binding;
	          });

	for (const auto &ub_rsrc : pass.uniform_buffer_rsrcs)
	{
		const auto &p_ub_info = dynamic_cast<VkUniBufUsageInfo &>(*RG.rsrc_usage_infos[ub_rsrc.second.handle]);

		const auto rsrc_uniform_buffer = dynamic_cast<Resource<VkBufferBase> *>(RG.resources[ub_rsrc.first.resource_handle]);

		uniform_buffer_infos.emplace_back(p_ub_info.dst_binding, p_ub_info.dst_array_element, rsrc_uniform_buffer->resource);
	}
	renderpass->GetUniformBufferDescriptorsRG(std::move(uniform_buffer_infos));
}

void RenderGraph::GraphicsPassNode::CreateGraphicsPipelineLayoutRG()
{
	renderpass->CreateGraphicsPipelineLayoutRG();
}

void RenderGraph::GraphicsPassNode::CreateShadersRG()
{
	renderpass->CreateShadersRG();
}

void RenderGraph::GraphicsPassNode::CreateGraphicsPipelineRG()
{
	renderpass->CreateGraphicsPipelineRG();
}
