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

RenderGraph::DependencyGraph::PassNodeHandle RenderGraph::DependencyGraph::AddGfxPassNode(std::shared_ptr<VkRenderpassBaseRG> renderpass, const std::string name, const std::vector<ResourceSlot> inputs, const std::vector<ResourceSlot> putputs)
{
	//TODO:�����inpus��putputs���а�ȫ�Լ�飬���һ����ԴҪ����2�����ϻ���д2�����ϣ��򱨴�
	//TODO:����ͬһ��pass ����д��д��� �����⡣����1��������inputs������putputs���ֱ��ʾ ����д �� д��� �������

	//CREATE PASSNODE
	auto p_pass_node              = std::make_unique<GraphicsPassNode>(nodes_of_passes.size());
	p_pass_node->node_handle      = nodes_of_passes.size();
	p_pass_node->renderpass       = renderpass;



	const auto &current_pass_node = nodes_of_passes.emplace_back(std::move(p_pass_node));

	//CREATE INCOMING EDGES TO CURRENT PASS NODE

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

	const auto result_handle = nodes_of_passes.size() - 1;
	//renderpass->SetRenderGraphPassHandle(result_handle);


	return result_handle;
}

void RenderGraph::DependencyGraph::RegisterRsrcsUsage(const PassNodeHandle pass_node_handle, const ResourceSlot rsrc_slot, std::unique_ptr<RsrcUsageInfoInPass> rsrc_info)
{
	const auto info_t = rsrc_info->GetInfoType();

	rsrc_usage_infos.push_back(std::move(rsrc_info));

	const RsrcUsageHandle rsrc_info_handle = rsrc_usage_infos.size() - 1;
	auto &                pass             = *nodes_of_passes[pass_node_handle];

	//��ʹ����Ϣ�ַ�����ͬ�������У�����һ����renderpass�н��ж�Ӧ�Ĵ�����������̫��ʱ����Ϊ����vector�Ķ���һ�ѡ��������

	if (Vk::RsrcInfoType::Attachment == info_t)
	{
		pass.attachment_rsrcs.emplace_back(rsrc_slot, RsrcUsageInfoSlot{rsrc_info_handle});
	}

	if (Vk::RsrcInfoType::Buffer == info_t)
	{
		pass.buffer_rsrcs.emplace_back(rsrc_slot, RsrcUsageInfoSlot{rsrc_info_handle});
	}

	if (Vk::RsrcInfoType::Texture == info_t)
	{
		pass.texture_rsrcs.emplace_back(rsrc_slot, RsrcUsageInfoSlot{rsrc_info_handle});
	}

	//ͬʱ��ȫ����Դʹ�ü�¼���� ��¼����Դ��ʹ��
	//��һ�£�һ����ԴҲ�� �ȱ�pass0ʹ�ã�Ȼ���ٱ�pass1ʹ�ã������û���RegisterRsrcsUsage���ܲ�ϲ����˳��������RegisterRsrcsUsage��pass1����RegisterRsrcsUsage��pass0��������ô��Ҫ��������������һ�飨֮�����ﻹ��Ҫ������Ϊʹ�÷�ʽRegisterRsrcsUsage��¼�ĵ��ò�һ���ǰ�˳��ģ�

	if (resources_usage_recording_table.contains(rsrc_slot))        //����� ��Դ��ʹ�õĵ�һ��pass�� �Ѿ���¼�˶�Ӧ��ʹ�÷�ʽ���򣺣�֮�����ﻹ��Ҫ������Ϊʹ�÷�ʽRegisterRsrcsUsage��¼�ĵ��ò�һ���ǰ�˳��ģ�
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
	if (nodes_of_passes.size() != pass_node_dependencies.size())
	{
		throw std::runtime_error("you shited your bed");
	}

	//Ԥ��pass_node_dependencies���Ⱥ�nodes_of_passesһ�£�
	for (size_t i = 0; i < nodes_of_passes.size(); ++i)
	{
		const auto &p_pass_node = nodes_of_passes[i];

		auto &current_pass_node_depen = pass_node_dependencies[i];

		for (const auto p_e : p_pass_node->incoming_edges)        //����������Ҫ����ǰpass�������Դ����
		{
			const auto &rsrc_node = nodes_of_resources[p_e.from];

			if (!rsrc_node->pass_accessing_order.empty())        //������Դ�����ʹ�ã���Ⱦͼ�г��ֻ��������֮����Ҫ����
			{
				//TODO: you konw the drill
			}

			for (const auto &incoming_edge_of_rsrc_node : rsrc_node->incoming_edges)        //����д�������Դ��pass node�����뵽dependency��ȥ
			{
				current_pass_node_depen.insert(incoming_edge_of_rsrc_node.from);
			}
		}
	}
}

bool RenderGraph::DependencyGraph::Move()
{
	return true;
}

void RenderGraph::DependencyGraph::Purge()
{

	slots_of_rsrcs.clear();
	resources.clear();
	nodes_of_resources.clear();
	nodes_of_passes.clear();
	rsrc_usage_infos.clear();
	rsrcs_map.clear();
	resources_usage_recording_table.clear();
	final_execution_order.clear();


}

bool RenderGraph::DependencyGraph::Compile()
{
	//TODO: Trim all invalid passes and unused resources

	final_execution_order.clear();
	final_execution_order.reserve(nodes_of_passes.size());

	std::vector<const PassNode *> pass_nodes_excluded;
	//std::vector<const PassNode *>           temp_final_execution_order;

	//pass_node_dependencies������±��ʾ�ڼ���pass��std::unordered_set<size_t>��ʾ���pass ����������Щpass.
	std::vector<std::unordered_set<size_t>> pass_node_dependencies(nodes_of_passes.size());
	GeneratePassNodeDepen(pass_node_dependencies);

	//Topological Sorting
	//step 1:�ҵ����Ϊ0��pass�ڵ㣬����ж��������ȡ���С�ģ�
	//step 2:���ýڵ�ı�������
	//step 3:���ýڵ�ɾ����ͬʱ������ �ɸýڵ�����������ɾ����
	//step 4:ѭ������ step 2 �� step 3 ��ֱ��ͼ�е�ͼ�����нڵ����ȶ�Ϊ�㣻
	//step 5:�������������

	//TODO:���߳��Ż�����ô������pass���в�λ��֣���ÿһ�����ֶ�������ȥ����ִ��

	while (true)
	{
		for (size_t i = 0; i < nodes_of_passes.size(); ++i)
		{
			const auto &p_pass_node = *nodes_of_passes[i];

			if (pass_nodes_excluded.end() == std::find(pass_nodes_excluded.begin(), pass_nodes_excluded.end(), &p_pass_node))        //��ǰpass node��û�б��Ƴ���ִ�ж��У�Ҳ����ִ��˳��δ������
			{
				if (pass_node_dependencies[i].empty())        //���Ϊ0��pass�ڵ�,��������ģ�
				{
					final_execution_order.push_back(i);        //����ִ��˳��
					//temp_final_execution_order.push_back(&p_pass_node);
					pass_nodes_excluded.push_back(&p_pass_node);
				}
				else        //��Ȳ���0��pass�ڵ�,��������ģ�
				{
					bool no_depen = true;        //��������Ȳ���0�Ľڵ㣬�������������ڵ�պ�[��]�Ѿ�������final execution order�У�Ҳ����ִ��˳���Ѿ������������Ƴ���ִ�ж����ˣ���

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
						//temp_final_execution_order.push_back(&p_pass_node);
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

	//MAKE SURE that passes within resources_usage_recording_table are in correct order ��һ�£�һ����ԴҲ�� �ȱ�pass0ʹ�ã�Ȼ���ٱ�pass1ʹ�ã������û���RegisterRsrcsUsage���ܲ�ϲ����˳��������RegisterRsrcsUsage��pass1����RegisterRsrcsUsage��pass0��������ô��Ҫ��������������һ��

	//��Դʹ��������
	//ÿ����Դһ��ֻ�ܱ�����passʹ�ã����ʹ�õ������Ҫ��pass accessing order����
	for (auto &[_, rsrc_usage_info_of_two_passes] : resources_usage_recording_table)
	{
		if (rsrc_usage_info_of_two_passes.front().first && rsrc_usage_info_of_two_passes.back().first)        //�е���Դ����ֻ�ᱻһ��passʹ��һ�Σ���ʱstd::optionalΪ��
		{
			if (!IfPassInOrder(rsrc_usage_info_of_two_passes.front().first.value(), rsrc_usage_info_of_two_passes.back().first.value()))
			{
				std::swap(rsrc_usage_info_of_two_passes.front(), rsrc_usage_info_of_two_passes.back());
			}
		}
	}

	//INFERENCE of the loading operation of ATTACHMENTs  �Ƶ�attachment��Դ�� ���� �� �洢 ����
	for (const auto &pass : nodes_of_passes)        //for all the passes
	{
		for (auto &attach : pass->attachment_rsrcs)        //take all the attachment rsrcs of the pass��֮ǰ�ַ�����ͬ�������������Դ�����������ȴ���attachments��Դ��
		{
			const auto &rsrc_node = *nodes_of_resources[attach.first.resource_handle];

			//����ʱcast
			auto &attach_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*rsrc_usage_infos[attach.second.handle]);
			//����std::array<std::pair<std::optional<PassNodeHandle>, RsrcUsageInfoSlot>, 2>
			const auto &rsrc_usage_info_of_two_passes = resources_usage_recording_table[attach.first];

			if (rsrc_node.pass_accessing_order.empty())        //no Resource Reuse�������Ҳ����ֻ������pass������ֻ��һ��pass��ʹ�������resources
			{
				if (pass == nodes_of_passes[rsrc_usage_info_of_two_passes.front().first.value()])        //��ǰpass��attachment��ʹ�õĵ�һ��pass
				{
					const auto accessing_type = nodes_of_passes[rsrc_usage_info_of_two_passes.front().first.value()]->rsrc_accessing_types[attach.first];
					if (VirtualResource::RsrcAccessType::Write == accessing_type)
					{
						attach_info.load_op  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;        //���滹û������
						attach_info.store_op = VK_ATTACHMENT_STORE_OP_STORE;
					}
				}
				else        //��ǰpass��attachment��ʹ�õĵڶ���pass
				{
					const auto accessing_type = nodes_of_passes[rsrc_usage_info_of_two_passes.back().first.value()]->rsrc_accessing_types[attach.first];
					if (VirtualResource::RsrcAccessType::Read == accessing_type)
					{
						attach_info.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
						attach_info.store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;        //��Ϊrsrc_usage_info_of_two_passes�ǿյģ�����������dont careû����
					}
				}
			}
			else        //pass_accessing_order��Ϊ�յ����
			{
				//TODO:
			}
		}
	}

	//INFERENCE of the ������Դʹ�õ��Ƶ���

	std::cout << "Render graph compilation finished" << std::endl;

	return true;
}

bool RenderGraph::DependencyGraph::Execute(VkCommandBuffer cmb)
{
	using namespace std;

	//��Դ��ʼ����Ϣ �� ͬ����Ϣ¼��

	for (auto rsrc_slot : slots_of_rsrcs)
	{
		const array<pair<optional<PassNodeHandle>, RsrcUsageInfoSlot>, 2> &pass_and_usage_idx_vec = resources_usage_recording_table[rsrc_slot];

		if (!pass_and_usage_idx_vec.front().first.has_value())        //���������һ��pass��û��ʹ����Դ����һ���ǳ������ˡ�
		{
			throw runtime_error("error!No pass uses this resource");
		}

		//��һ��ʹ�õ�ǰ��Դ��pass
		auto &first_pass = *nodes_of_passes[pass_and_usage_idx_vec.front().first.value()];

		//*****************************************************************************************

		if (nodes_of_resources[rsrc_slot.node_handle]->pass_accessing_order.empty())   //��Դ������ظ�ʹ�õ����
		{
			//DECIDING WHICH RESOURCES NEED TO BE INITIALIZED WITHIN THE PASS
			if (VirtualResource::RsrcLifeTimeType::Persistent != resources[rsrc_slot.resource_handle]->life_time)        // persistent resources are imported from outside RG,don't need to be init-ed or destoryed
			{
				//DECIDING at which pass THE GIVEN resources CAN BE INIT
				first_pass.slot_rsrc_init_list.push_back(rsrc_slot);

				//DECIDING at which pass THE GIVEN resources CAN BE DESTROYED
				if (pass_and_usage_idx_vec.back().first)        //����еڶ���pass����Դ�����˶�����д����ô˵����ǰ��Դ��Ҫ�ڵڶ���ʹ������pass�����Ժ��������
				{
					auto &second_pass = *nodes_of_passes[pass_and_usage_idx_vec.back().first.value()];
					second_pass.slot_rsrc_recycle_list.push_back(rsrc_slot);        //����������recycle list������destroy��Ϊ��֮���������Ż���׼����
				}
				else
				{
					first_pass.slot_rsrc_recycle_list.push_back(rsrc_slot);
				}

			}

			//RECORDING at which pass RESOURCES need STATE CHANGES
			first_pass.slot_rsrc_state_changing_list.push_back(rsrc_slot);
			if (pass_and_usage_idx_vec.back().first)  //����еڶ���passʹ�õ�ǰ��Դ�Ļ���
			{
				auto &second_pass = *nodes_of_passes[pass_and_usage_idx_vec.back().first.value()];
				second_pass.slot_rsrc_state_changing_list.push_back(rsrc_slot);
			}
			else   //��Դ������ظ�ʹ�õ����
			{
			}
		}
		else
		{



		}




		//*****************************************************************************************
		//RECORDING THE SYNC INFORMATION

		//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ
		Vk::SyncInfo syn_info_source = rsrc_usage_infos[pass_and_usage_idx_vec.front().second.handle]->GetSynInfo();

		//target info ���Ǵӵڶ���pass������ȡ������ usage��Ϣ��Ҳ��ڶ���pass�����ڣ�but that's fine��������һ��Ĭ�ϵ� ;) ��

		Vk::SyncInfo syn_info_target{
		    .access_mask   = VK_ACCESS_NONE,
		    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

		if (pass_and_usage_idx_vec.back().first)//����еڶ���passʹ��ͬһ����Դ
		{
			auto &second_pass = *nodes_of_passes[pass_and_usage_idx_vec.back().first.value()];


			syn_info_target   = rsrc_usage_infos[pass_and_usage_idx_vec.back().second.handle]->GetSynInfo();
			second_pass.syn_infos.emplace_back(std::tuple{syn_info_source, syn_info_target, rsrc_slot});

		}
		else//���û�еڶ���passʹ��ͬһ����Դ
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


	std::cout << "Render graph command recording finished" << std::endl;
	return true;


}








void RenderGraph::DependencyGraph::ExecutePass(VkCommandBuffer cmd_buf, const PassNodeHandle pass_node_handle)
{

	auto &pass = *nodes_of_passes[pass_node_handle];


	//REQUIRED RESOURCES INITIALIZATION
	for (const auto &slot : pass.slot_rsrc_init_list)
	{

		// persistent resources are imported from outside RG,don't need to be init-ed or destoryed
		if (VirtualResource::RsrcLifeTimeType::Persistent != resources[slot.resource_handle]->life_time)        		
		{
			//VirtualResource has a descriptor to help initalize resources
			auto& pvr = resources[slot.resource_handle];
			pvr->Actualize(renderpass_manager);
		}

	}

	//CHANGE STATES OF ALL RESOURCES THAT NEED TO HAVE NEW STATE WITHIN THE CURRENT PASS
	for (const auto rsrc_slot : pass.slot_rsrc_state_changing_list)
	{
		const std::array< std::pair<std::optional<PassNodeHandle>, RsrcUsageInfoSlot>, 2 > &pass_node_handle_and_usage_handle_vec = resources_usage_recording_table[rsrc_slot];

		//���ݵ�ǰpass��ʹ����Դ�ĵ�һ��pass
		if (pass_node_handle == pass_node_handle_and_usage_handle_vec.front().first.value())
		{
			const auto& pvr = resources[rsrc_slot.resource_handle];
			pvr->StateChangeNoNeedCmdRecording(renderpass_manager, *rsrc_usage_infos[pass_node_handle_and_usage_handle_vec.front().second.handle]);
		}

		//���ݵ�ǰpass��ʹ����Դ�ĵڶ���pass
		if (pass_node_handle_and_usage_handle_vec.back().first && (pass_node_handle == pass_node_handle_and_usage_handle_vec.back().first.value()))
		{
			const auto& pvr = resources[rsrc_slot.resource_handle];
			pvr->StateChangeNoNeedCmdRecording(renderpass_manager, *rsrc_usage_infos[pass_node_handle_and_usage_handle_vec.back().second.handle]);
		}

	}



	//BARRIERS INSERSION before PASS EXECUTION
	for (auto &[source_syn, target_syn, resource_slot] : pass.syn_infos)
	{
		const auto& pvr = resources[resource_slot.resource_handle];
		//��̬������ȷ��InsertSync��������ͬ��Դinsert barrier�ķ�����ͬ��
		pvr->InsertSync(cmd_buf, source_syn, target_syn);
	}

	//RENDERPASS INITIALIZATION AND EXECUTION
	pass.PrePassExecuteRG(*this);
	pass.ExecuteRG(cmd_buf);



	//TRANSIENT RESOURCES DESTRUCTION
	for (const auto &slot : pass.slot_rsrc_recycle_list)
	{
		// persistent resources are imported from outside RG,don't need to be init-ed or destoryed
		if (VirtualResource::RsrcLifeTimeType::Persistent != resources[slot.resource_handle]->life_time)        		
		{
			//VirtualResource has a descriptor to help initalize resources
			auto& pvr = resources[slot.resource_handle];
			pvr->DeActualize(renderpass_manager);
		}

	}




	//TODO: BARRIERS INSERSION after Pass��Ҳ����Ҫ��pass֮�����barrier��
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




VkAttachmentInfo RenderGraph::DependencyGraph::GetAttachmentRsrc(std::string name, PassNodeHandle pass_node_handle)
{
	//�����ҵ���ǰpass��handle
	const auto &cur_pass_node = nodes_of_passes[pass_node_handle];
	//�ҵ���Դ��Ӧ��slot
	const auto rsrcs_slot = rsrcs_map[name];
	//����Դslot��Ϊ��ֵ���ҵ���Դ��ʹ�÷�ʽ��
	const std::array<std::pair<std::optional<RenderGraph::DependencyGraph::PassNodeHandle>, RenderGraph::RsrcUsageInfoSlot>, 2> rsrc_usage = resources_usage_recording_table[rsrcs_slot];

	//�ҵ���ǰpass ��Դ��ʹ�õķ�ʽ
	if (rsrc_usage.front().first.has_value())
	{
		if (rsrc_usage.front().first.value() == cur_pass_node->node_handle)
		{
			const auto &                       attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*rsrc_usage_infos[rsrc_usage.front().second.handle]);
			const VkAttachmentInfo::WithinPass attach_info_inpass{attch_info};
			const auto &                       rsrc_tex = dynamic_cast<RenderGraph::Resource<VkTexture> &>(*resources[rsrcs_slot.resource_handle]);
			return VkAttachmentInfo{attach_info_inpass, rsrc_tex.resource};
		}
	}
	else if (rsrc_usage.back().first.has_value())
	{
		if (rsrc_usage.back().first.value() == cur_pass_node->node_handle)
		{
			const auto &                       attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*rsrc_usage_infos[rsrc_usage.back().second.handle]);
			const VkAttachmentInfo::WithinPass attach_info_inpass{attch_info};
			const auto &                       rsrc_tex = dynamic_cast<RenderGraph::Resource<VkTexture> &>(*resources[rsrcs_slot.resource_handle]);
			return VkAttachmentInfo{attach_info_inpass, rsrc_tex.resource};
		}
	}
	else
	{
		throw std::runtime_error("Can't find resources usage info");
	}


}

VkTexUsageInfo RenderGraph::DependencyGraph::GetTextureRsrc(std::string name, PassNodeHandle pass_node_handle)
{
	//�����ҵ���ǰpass��handle
	const auto &cur_pass_node = nodes_of_passes[pass_node_handle];
	//�ҵ���Դ��Ӧ��slot
	const auto rsrcs_slot = rsrcs_map[name];
	//����Դslot��Ϊ��ֵ���ҵ���Դ��ʹ�÷�ʽ��
	const std::array<std::pair<std::optional<RenderGraph::DependencyGraph::PassNodeHandle>, RenderGraph::RsrcUsageInfoSlot>, 2> rsrc_usage = resources_usage_recording_table[rsrcs_slot];

	if (rsrc_usage.front().first.has_value())
	{
		//make sure cur_pass_node->node_handle and pass_handle_inRG are the same.
		if (rsrc_usage.front().first.value() == cur_pass_node->node_handle)
		{
			const auto &tex_usage_info = dynamic_cast<VkTexUsageInfoRG &>(*rsrc_usage_infos[rsrc_usage.front().second.handle]);
			const auto &rsrc_tex       = dynamic_cast<RenderGraph::Resource<VkTexture> &>(*resources[rsrcs_slot.resource_handle]);

			return VkTexUsageInfo{tex_usage_info.tex_type, tex_usage_info.stage_mask, tex_usage_info.set_info, rsrc_tex.resource};
		}
	}
	else if (rsrc_usage.back().first.has_value())
	{
		if (rsrc_usage.back().first.value() == cur_pass_node->node_handle)
		{
			const auto &tex_usage_info = dynamic_cast<VkTexUsageInfoRG &>(*rsrc_usage_infos[rsrc_usage.back().second.handle]);
			const auto &rsrc_tex       = dynamic_cast<RenderGraph::Resource<VkTexture> &>(*resources[rsrcs_slot.resource_handle]);
			return VkTexUsageInfo{tex_usage_info.tex_type, tex_usage_info.stage_mask, tex_usage_info.set_info, rsrc_tex.resource};
		}
	}
	else
	{
		throw std::runtime_error("Can't find resources usage info");
	}
}

VkUniBufUsageInfo RenderGraph::DependencyGraph::GetBufferRsrc(std::string name, PassNodeHandle pass_node_handle)
{
	//�����ҵ���ǰpass��handle
	const auto &cur_pass_node = nodes_of_passes[pass_node_handle];
	//�ҵ���Դ��Ӧ��slot
	const auto rsrcs_slot = rsrcs_map[name];
	//����Դslot��Ϊ��ֵ���ҵ���Դ��ʹ�÷�ʽ��
	const std::array<std::pair<std::optional<RenderGraph::DependencyGraph::PassNodeHandle>, RenderGraph::RsrcUsageInfoSlot>, 2> rsrc_usage = resources_usage_recording_table[rsrcs_slot];

	if (rsrc_usage.front().first.has_value())
	{
		//make sure cur_pass_node->node_handle and pass_handle_inRG are the same.
		if (rsrc_usage.front().first.value() == cur_pass_node->node_handle)
		{
			const auto &buf_usage_info = dynamic_cast<VkBufUsageInfoRG &>(*rsrc_usage_infos[rsrc_usage.front().second.handle]);
			const auto &rsrc_buffer    = dynamic_cast<RenderGraph::Resource<VkBufferBase> &>(*resources[rsrcs_slot.resource_handle]);

			return VkUniBufUsageInfo{buf_usage_info.buf_type, buf_usage_info.stage_mask, buf_usage_info.set_info, rsrc_buffer.resource};
		}
	}
	else if (rsrc_usage.back().first.has_value())
	{
		if (rsrc_usage.back().first.value() == cur_pass_node->node_handle)
		{
			const auto &buf_usage_info = dynamic_cast<VkBufUsageInfoRG &>(*rsrc_usage_infos[rsrc_usage.back().second.handle]);
			const auto &rsrc_buffer    = dynamic_cast<RenderGraph::Resource<VkBufferBase> &>(*resources[rsrcs_slot.resource_handle]);

			return VkUniBufUsageInfo{buf_usage_info.buf_type, buf_usage_info.stage_mask, buf_usage_info.set_info, rsrc_buffer.resource};
		}
	}
	else
	{
		throw std::runtime_error("Can't find resources usage info");
	}
}







RenderGraph::VirtualResource::VirtualResource(RsrcLifeTimeType life_time_) :
    life_time(life_time_)
{
}

RenderGraph::PassNode::PassNode(size_t node_handle_) :
    Node(node_handle_)
{
}

RenderGraph::GraphicsPassNode::GraphicsPassNode(size_t node_handle_) :
    PassNode(node_handle_)
{
}




void RenderGraph::GraphicsPassNode::PrePassExecuteRG(DependencyGraph &RG)
{
	//CreateAttachmentsRG(*this, RG);
	//CreateTexutreRG(*this, RG);
	//CreateBufferDescriptorsRG(*this, RG);

	renderpass->Init();

}

void RenderGraph::GraphicsPassNode::ExecuteRG(VkCommandBuffer cmd_buf)
{

	renderpass->Execute(cmd_buf);
}







//Abandoned Functions:
void RenderGraph::GraphicsPassNode::GiveAttachmentsToPass(RenderGraph::PassNode &pass, DependencyGraph &RG)
{
	//std::vector<VkAttachmentInfo> attachment_infos;
 //
	////attachments �� texture �ᱻ�ַ�����ͬ��������ȥ������������԰�std::variantֱ��ת����AttachmentIndex
	////��Ϊattachment resources��һ��AttachmentIndex�����Ը������AttachmentIndex����
	//std::sort(pass.attachment_rsrcs.begin(), pass.attachment_rsrcs.end(),
	//          [&RG](const std::pair<ResourceSlot, RsrcUsageInfoSlot> &lhs, const std::pair<ResourceSlot, RsrcUsageInfoSlot> &rhs) -> bool {

	//	          const auto &lhs_attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*RG.rsrc_usage_infos[lhs.second.handle]);
	//	          const auto &rhs_attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*RG.rsrc_usage_infos[rhs.second.handle]);
	//	          return std::get<Vk::AttachmentIndexType>(lhs_attch_info.slot_info) > std::get<Vk::AttachmentIndexType>(rhs_attch_info.slot_info);
	//          });

	//for (const auto &attach_rsrc : pass.attachment_rsrcs)
	//{
	//	const auto &attch_info = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(*RG.rsrc_usage_infos[attach_rsrc.second.handle]);

	//	const VkAttachmentInfo::WithinPass attach_info_inpass{attch_info};

	//	const auto rsrc_tex = dynamic_cast<Resource<VkTexture>* >(RG.resources[attach_rsrc.first.resource_handle]);

	//	attachment_infos.emplace_back(VkAttachmentInfo{attach_info_inpass, rsrc_tex->resource});
	//}

	//renderpass->GetAttachmentsRG(std::move(attachment_infos));
}


void RenderGraph::GraphicsPassNode::GiveTexturesToPass(RenderGraph::PassNode &pass, DependencyGraph &RG)
{

	//std::vector<VkTexUsageInfo> tex_infos;

	//for (const auto &tex_rsrc : pass.texture_rsrcs)
	//{
	//	//usage info
	//	const auto &tex_info = dynamic_cast<VkTexUsageInfoRG &>(*RG.rsrc_usage_infos[tex_rsrc.second.handle]);

	//	//const auto &tex_infos = static_cast<VkTexUsageInfoRG &>(*RG.rsrc_usage_infos[tex_rsrc.second.handle]);

	//	//actual rsrc
	//	const auto rsrc_tex = dynamic_cast<Resource<VkTexture>* >(RG.resources[tex_rsrc.first.resource_handle]);

	//	tex_infos.emplace_back(VkTexUsageInfo{
	//		tex_info.tex_type,
	//	tex_info.stage_mask,
	//		tex_info.set_info,
	//	rsrc_tex->resource
	//	}
	//	
	//	);
	//}

	//renderpass->GetTexturesRG(std::move(tex_infos));
}



void RenderGraph::GraphicsPassNode::GiveBuffersToPass(RenderGraph::PassNode &pass, DependencyGraph &RG)
{


	//std::vector<VkUniBufUsageInfo> uniform_buffer_infos;

	////std::sort(pass.uniform_buffer_rsrcs.begin(), pass.uniform_buffer_rsrcs.end(),
	////          [&RG](const std::pair<ResourceSlot, RsrcUsageInfoSlot> &lhs, const std::pair<ResourceSlot, RsrcUsageInfoSlot> &rhs) -> bool {
	////	          const auto &lhs_attch_info = dynamic_cast<VkBufferBase::WithinPassRG &>(*RG.rsrc_usage_infos[lhs.second.handle]);
	////	          const auto &rhs_attch_info = dynamic_cast<VkBufferBase::WithinPassRG &>(*RG.rsrc_usage_infos[rhs.second.handle]);

	////	          if (lhs_attch_info.slot_info.binding == rhs_attch_info.slot_info.binding)
	////	          {
	////		          return lhs_attch_info.slot_info.array_elemnt > rhs_attch_info.slot_info.array_elemnt;
	////	          }

	////	          return lhs_attch_info.slot_info.binding > rhs_attch_info.slot_info.binding;
	////          });

	//for (const auto &ub_rsrc : pass.buffer_rsrcs)
	//{
	//	const auto &ub_info = dynamic_cast<VkBufUsageInfoRG &>(*RG.rsrc_usage_infos[ub_rsrc.second.handle]);

	//	const auto rsrc_uniform_buffer = dynamic_cast<Resource<VkBufferBase> *>(RG.resources[ub_rsrc.first.resource_handle]);

	//	uniform_buffer_infos.emplace_back( ub_info.buf_type, ub_info.stage_mask, ub_info.set_info ,rsrc_uniform_buffer->resource);
	//}


	//renderpass->GetUniformBufferDescriptorsRG(std::move(uniform_buffer_infos));



}








