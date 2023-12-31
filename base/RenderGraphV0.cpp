#include "RenderGraphV0.h"

RenderGraphV0::DependencyGraph::DependencyGraph(VkRenderpassManager &renderpass_manager_) :
    renderpass_manager(renderpass_manager_)
{
}

VkAttachmentInfo RenderGraphV0::DependencyGraph::GetAttachmentRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_uni_tex.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &     virtual_rsrc = pass->ins_uni_tex.at(inlet_name);
	const auto rsrc_usage   = virtual_rsrc.GetRsrcUsage();

	const VkAttachmentInfo::WithinPass attach_info_inpass{*rsrc_usage};

	return VkAttachmentInfo{attach_info_inpass, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkAttachmentInfo RenderGraphV0::DependencyGraph::GetAttachmentRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_uni_tex.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetRsrcUsage();

	const VkAttachmentInfo::WithinPass attach_info_inpass{*rsrc_usage};
	return VkAttachmentInfo{attach_info_inpass, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkTexUsageInfo RenderGraphV0::DependencyGraph::GetTextureRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_uni_tex.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &virtual_rsrc = pass->ins_uni_tex.at(inlet_name);

	const auto &rsrc_usage = virtual_rsrc.GetRsrcUsage();

	return VkTexUsageInfo{rsrc_usage->tex_type, rsrc_usage->shader_stages, std::get<Vk::DescSetInfo>(rsrc_usage->binding_info), virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkTexUsageInfo RenderGraphV0::DependencyGraph::GetTextureRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_uni_tex.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetRsrcUsage();

	return VkTexUsageInfo{rsrc_usage->tex_type, rsrc_usage->shader_stages, std::get<Vk::DescSetInfo>(rsrc_usage->binding_info), virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkUniBufUsageInfo RenderGraphV0::DependencyGraph::GetBufferRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_buf.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->ins_buf.at(inlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetRsrcUsage();

	return VkUniBufUsageInfo{rsrc_usage->buf_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkUniBufUsageInfo RenderGraphV0::DependencyGraph::GetBufferRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_buf.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_buf.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetRsrcUsage();

	return VkUniBufUsageInfo{rsrc_usage->buf_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingAttachmentOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_uni_tex.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &     virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
	const auto rsrc_usage   = virtual_rsrc.GetRsrcUsage();
	auto &     tex_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingAttachmentIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_uni_tex.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &     virtual_rsrc = pass->ins_uni_tex.at(inlet_name);
	const auto rsrc_usage   = virtual_rsrc.GetRsrcUsage();

	auto &tex_vrsrc = virtual_rsrc.GetItrInRsrcMap()->second;
	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingTextureIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_uni_tex.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &virtual_rsrc = pass->ins_uni_tex.at(inlet_name);

	const auto &rsrc_usage = virtual_rsrc.GetRsrcUsage();
	auto &      tex_vrsrc  = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingTextureOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_uni_tex.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_uni_tex.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetRsrcUsage();
	auto &      tex_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingBufferIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_buf.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->ins_buf.at(inlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetRsrcUsage();
	auto &      buf_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, buf_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingBufferOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_buf.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_buf.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetRsrcUsage();
	auto &      buf_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, buf_vrsrc, *rsrc_usage);
}

RenderGraphV0::GraphicsPassNode &RenderGraphV0::DependencyGraph::AddGfxPassNode(std::string name, std::shared_ptr<VkRenderpassBaseRG> renderpass_ptr)
{
	auto gfx_node = std::make_unique<GraphicsPassNode>(name, renderpass_ptr, this->buffers_map, this->textures_map, this->attachments_map);
	gfx_pass_nodes.emplace_back(std::move(gfx_node));
	const auto &result = gfx_pass_nodes.back();
	renderpass_ptr->SetRenderGraphPassHandle(gfx_pass_nodes.size() - 1u);
	renderpass_ptr->SetRenderGraphPassPointer(result.get());

	return *result;
}

bool RenderGraphV0::DependencyGraph::Compile(const VkDeviceManager &device_man)
{
	//TODO: Trim all invalid passes and unused resources

	//清除最后的执行顺序
	final_execution_order_hint.clear();
	final_execution_order_hint.reserve(gfx_pass_nodes.size());

	GeneratePassNodeDepen();        //由In,Out函数声明确定可以推断出来的顺序

	//拓扑排序算法1，dfs
	std::vector<bool> visited(gfx_pass_nodes.size(), false);
	std::vector<bool> on_stack(gfx_pass_nodes.size(), false);

	for (auto gfx_node_itr = gfx_pass_nodes.begin(); gfx_node_itr < gfx_pass_nodes.end(); ++gfx_node_itr)
	{
		auto index = std::distance(gfx_pass_nodes.begin(), gfx_node_itr);
		VisitNode(index, gfx_node_itr->get(), visited, on_stack, topologically_sorted_nodes);

		if (topologically_sorted_nodes.size() == gfx_pass_nodes.size())
		{
			break;
		}
	}

	std::ranges::reverse(topologically_sorted_nodes);

	//拓扑排序算法2
	////Topological Sorting拓扑排序，为mark root pass做准备
	////step 1:找到入度为0的pass节点，如果有多个，则先取编号小的；
	////step 2:将该节点的编号输出；
	////step 3:将该节点删除，同时将所有 由该节点出发的有向边删除；
	////step 4:循环进行 step 2 和 step 3 ，直到图中的图中所有节点的入度都为零；
	////step 5:拓扑排序结束；
	//while (true)
	//{
	//	for (const auto &p_pass_node : gfx_pass_nodes)
	//	{
	//		auto &pass_node = *p_pass_node;
	//		if (final_execution_order_hint.end() == std::find(final_execution_order_hint.begin(), final_execution_order_hint.end(), &pass_node))        //当前pass node还没有被移除到执行队列，也就是执行顺序还未定下来
	//		{
	//			if (pass_node.passes_depen_set.empty())        //入度为0的pass节点,运行下面的：
	//			{
	//				final_execution_order_hint.push_back(&pass_node);        //加入最终的执行顺序
	//			}
	//			else        //入度不是0的pass节点,运行下面的：
	//			{
	//				bool no_depen = true;        //如果这个入度不是0的节点，所依赖的其他节点刚好[都]已经放入了final execution order中（也就是执行顺序已经被决定;被移到执行队列了）；

	//				for (const auto depen : pass_node.passes_depen_set)
	//				{
	//					if (final_execution_order_hint.end() == std::find(final_execution_order_hint.begin(), final_execution_order_hint.end(), depen))
	//					{
	//						no_depen = false;
	//						break;
	//					}
	//				}

	//				if (no_depen)
	//				{
	//					final_execution_order_hint.push_back(&pass_node);        //加入执行顺序
	//				}
	//			}
	//		}
	//	}
	//	if (final_execution_order_hint.size() == gfx_pass_nodes.size())
	//	{
	//		break;
	//	}
	//}

	//Again, we begin at the bottom-most root pass, and note all the resources that get read by this pass. Then we iterate upwards in our list of passes until we find a pass which writes to any of these read resources. If we find such a pass, it is tagged as contributing to that particular root pass.

	//There’s one special case: if a pass uses a resource write-only, we take this resource out of our reads-set - we won’t be interested in the next-earlier write to this resource as it will get overwritten anyway.

	//We add the read resources of our contributing pass to our current set of monitored read resources and continue our iteration upwards, until we find the next pass which writes to any of these read resources. If we find such a pass, it too contributes to the current root pass and we add a matching tag to it.

	//多queue优化，继续对pass进行层次划分，变成pass groups(一个个sub graph)，让每一个sub graph都可以拿去到不同的queue去并行执行

	///************************************************************
	//层次划分DEPENDENCY LEVELS GENERATION:
	///************************************************************
	std::vector<size_t> maximum_recursion_depth(topologically_sorted_nodes.size(), 0);
	for (auto pas_itr = topologically_sorted_nodes.begin(); pas_itr < topologically_sorted_nodes.end(); ++pas_itr)
	{
		size_t cur_index = std::distance(topologically_sorted_nodes.begin(), pas_itr);
		for (auto adjacent_pass : (*pas_itr)->passes_depen_on_cur_pass_set)
		{
			// Use std::find_if with a lambda function
			auto it_adj_pass = std::ranges::find_if(topologically_sorted_nodes,
			                                        [adjacent_pass](const PassNode *node_itr) {
				                                        return node_itr == adjacent_pass;
			                                        });
			if (it_adj_pass == topologically_sorted_nodes.end())
			{
				throw std::runtime_error("no pass found!");
			}

			size_t adj_index = std::distance(topologically_sorted_nodes.begin(), it_adj_pass);
			if (maximum_recursion_depth[adj_index] < maximum_recursion_depth[cur_index] + 1)
			{
				maximum_recursion_depth[adj_index] = maximum_recursion_depth[cur_index] + 1;
			}
		}
	}

	for (auto i = 0; i < topologically_sorted_nodes.size(); ++i)
	{
		topologically_sorted_nodes[i]->maximum_recursion_depth = maximum_recursion_depth[i];
	}
	const auto max_level_itr = std::ranges::max_element(maximum_recursion_depth);
	max_level                = *max_level_itr;
	///************************************************************
	//DISTRIBUTE WORK TO MULTIPLE QUEUES ONTO GPU:
	///************************************************************

	//std::unordered_map<VkDeviceManager::VkExecutionQueue *, std::list<PassNode *>>         queue_with_all_passes_on_it;
	//std::unordered_map<PassNode *, std::pair<VkDeviceManager::VkExecutionQueue *, size_t>> pass_on_queue_info;

	device_man.QueuesNumDisbatchedPassesToZero();
	size_t monotonical_assigned_index = 1;
	for (auto level = 0; level <= max_level; ++level)
	{
		for (auto &ptr_pass : topologically_sorted_nodes)
		{
			if (ptr_pass->maximum_recursion_depth == level)
			{
				VkRenderpassBaseRG::Type         pass_type = ptr_pass->GetRenderpassType();
				VkDeviceManager::QueueCapability queue_cap_requirement;
				if (pass_type & VkRenderpassBaseRG::Type::Graphics)
				{
					queue_cap_requirement.graphics = true;
				}

				if (pass_type & VkRenderpassBaseRG::Type::Compute)
				{
					queue_cap_requirement.compute = true;
				}

				if (pass_type & VkRenderpassBaseRG::Type::Transfor)
				{
					queue_cap_requirement.transfer = true;
				}

				if (pass_type & VkRenderpassBaseRG::Type::Present)
				{
					queue_cap_requirement.present = true;
				}
				auto &target_queue = device_man.GetSuitableQueue2(queue_cap_requirement);
				auto  ptr_queue    = &target_queue;
				if (queue_with_all_passes_on_it.contains(ptr_queue))
				{
					queue_with_all_passes_on_it.at(ptr_queue).push_back(ptr_pass);
				}
				else
				{
					queue_with_all_passes_on_it.emplace(ptr_queue, std::list<PassNode *>{ptr_pass});
				}

				auto result_itr = pass_on_queue_info.emplace(ptr_queue, std::pair<VkDeviceManager::VkExecutionQueue *, size_t>{ptr_queue, monotonical_assigned_index});

				if (!result_itr.second)
				{
					throw std::runtime_error("the pass has been distributed to queue!");
				}
				monotonical_assigned_index++;
			}
		}
	}

	std::vector<VkDeviceManager::VkExecutionQueue *> all_queue_in_use;        //确定一个queue的顺序，方便之后用index索引
	// Extract keys from the map and store them in the vector
	std::ranges::transform(queue_with_all_passes_on_it, std::back_inserter(all_queue_in_use),
	                       [](const std::pair<VkDeviceManager::VkExecutionQueue *, std::list<PassNode *>> &pair) {
		                       return pair.first;        // Extract the key
	                       });

	//FOR EVERY pass pair, we assign it an event, in order to do sync within a same queue;
	std::unordered_map<PassPair, VkEvent>     same_queue_sync_event;
	std::unordered_map<PassPair, VkSemaphore> diff_queue_sync_sema;
	VkEvent                                   shit_event;
	VkSemaphore                               shit_sema;
	for (auto itr = topologically_sorted_nodes.begin(); std::next(itr) != topologically_sorted_nodes.end(); ++itr)
	{
		same_queue_sync_event.emplace(PassPair{*itr, *std::next(itr)}, shit_event);
		diff_queue_sync_sema.emplace(PassPair{*itr, *std::next(itr)}, shit_sema);
	}

	//按照level层次来loop，
	for (auto level = 0; level <= *max_level_itr; ++level)
	{
		std::ranges::for_each(
		    topologically_sorted_nodes,
		    [level, this, &same_queue_sync_event, &diff_queue_sync_sema](PassNode *cur_pass) {
			    if (cur_pass->maximum_recursion_depth == level)
			    {
				    for (auto &[_, cur_out_let] : cur_pass->outs_buf)
				    {
					    for (auto &[tar_pass, tar_let] : cur_out_let.target_outlet_itrs)
					    {
						    auto  cur_pass_queue_cap = pass_on_queue_info.at(cur_pass).first->GetQueueCap();
						    auto  tar_pass_queue_cap = pass_on_queue_info.at(tar_pass).first->GetQueueCap();
						    auto &providing_outlet   = cur_out_let;
						    auto &target_let         = tar_let->second;
						    //Cur pass     ------------------------->       Tar pass

						    //Sync primitive
						    auto sync_event = same_queue_sync_event.at(PassPair{cur_pass, tar_pass});
						    auto sync_sema  = diff_queue_sync_sema.at(PassPair{cur_pass, tar_pass});

						    //CASE 1: in a same queue situation
						    if (cur_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index)
						    {
							    cur_pass->InsertSameQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_event,
							        cur_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());

							    tar_pass->InsertSameQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_event,
							        tar_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());
						    }
						    //CASE 2: NOT in a same queue situation (need queue ownership transfer!)
						    else
						    {
							    cur_pass->InsertDiffQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_sema,
							        cur_pass_queue_cap,
							        tar_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());

							    tar_pass->InsertSameQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_event,
							        tar_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());
						    }
					    }
				    }
				    //TODO: do the same shit for textures

				    for (auto &[_, cur_out_let] : cur_pass->outs_uni_tex)
				    {
					    for (auto &[tar_pass, tar_let] : cur_out_let.target_outlet_itrs)
					    {
						    auto  cur_pass_queue_cap = pass_on_queue_info.at(cur_pass).first->GetQueueCap();
						    auto  tar_pass_queue_cap = pass_on_queue_info.at(tar_pass).first->GetQueueCap();
						    auto &providing_outlet   = cur_out_let;
						    auto &target_let         = tar_let->second;
						    //Cur pass     ------------------------->       Tar pass

						    //Sync primitive
						    auto sync_event = same_queue_sync_event.at(PassPair{cur_pass, tar_pass});
						    auto sync_sema  = diff_queue_sync_sema.at(PassPair{cur_pass, tar_pass});

						    //CASE 1: in a same queue situation
						    if (cur_pass_queue_cap.queue_family_index == tar_pass_queue_cap.queue_family_index)
						    {
							    cur_pass->InsertSameQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_event,
							        cur_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());

							    tar_pass->InsertSameQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_event,
							        tar_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());
						    }
						    //CASE 2: NOT in a same queue situation (need queue ownership transfer!)
						    else
						    {
							    cur_pass->InsertDiffQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_sema,
							        cur_pass_queue_cap,
							        tar_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());

							    tar_pass->InsertSameQueueSyncInfo(
							        cur_pass,
							        tar_pass,
							        sync_event,
							        tar_pass_queue_cap,
							        providing_outlet.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetRsrcUsage()->GetSynInfo(),
							        target_let.GetUnderlyingRsrcItr());
						    }
					    }
				    }

				    //for (const auto later_pass : cur_pass->passes_depen_on_cur_pass_set)
				    //{
				    // auto &later_pass_queue_info = pass_on_queue_info.at(later_pass);
				    // auto &cur_pass_queue_info   = pass_on_queue_info.at(cur_pass);

				    // //Cur pass     ------------------------->       Later pass

				    // //CASE 1: in a same queue situation
				    // //*********************************
				    // for (auto &[_, inlet] : later_pass->ins_buf)
				    // {
				    //  //  Same queue here: || cur_pass  ->  set_event->  wait_event  ->  later_pass ||
				    //  auto providing_itr_and_pass = inlet.GetProvidingOutletItrAndPass();
				    //  if (providing_itr_and_pass)
				    //  {
				    //   if (providing_itr_and_pass.value().first == cur_pass)
				    //   {
				    //    auto &providing_outlet = providing_itr_and_pass.value().second->second;

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        inlet.GetRsrcUsage()->GetSynInfo(),
				    //        inlet.GetUnderlyingRsrcItr());

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        inlet.GetRsrcUsage()->GetSynInfo(),
				    //        inlet.GetUnderlyingRsrcItr());
				    //   }
				    //  }
				    // }

				    // for (auto &[_, later_let] : later_pass->ins_uni_tex)
				    // {
				    //  //  Same queue here: || cur_pass  ->  setevent->  waitevent  ->  later_pass ||
				    //  auto providing_itr_and_pass = later_let.GetProvidingOutletItrAndPass();

				    //  if (providing_itr_and_pass)
				    //  {
				    //   if (providing_itr_and_pass.value().first == cur_pass)
				    //   {
				    //    auto &providing_outlet = providing_itr_and_pass.value().second->second;

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetUnderlyingRsrcItr());

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetUnderlyingRsrcItr());
				    //   }
				    //  }
				    // }

				    // for (auto &[_, later_let] : later_pass->outs_buf)
				    // {
				    //  //  Same queue here: || cur_pass  ->  setevent->  waitevent  ->  later_pass ||
				    //  auto providing_itr_and_pass = later_let.GetProvidingOutletItrAndPass();
				    //  if (providing_itr_and_pass)
				    //  {
				    //   if (providing_itr_and_pass.value().first == cur_pass)
				    //   {
				    //    auto &providing_outlet = providing_itr_and_pass.value().second->second;

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetUnderlyingRsrcItr());

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetUnderlyingRsrcItr());
				    //   }
				    //  }
				    // }

				    // for (auto &[_, later_let] : later_pass->outs_uni_tex)
				    // {
				    //  //  Same queue here: || cur_pass  ->  setevent->  waitevent  ->  later_pass ||
				    //  auto providing_itr_and_pass = later_let.GetProvidingOutletItrAndPass();

				    //  if (providing_itr_and_pass)
				    //  {
				    //   if (providing_itr_and_pass.value().first == cur_pass)
				    //   {
				    //    auto &providing_outlet = providing_itr_and_pass.value().second->second;

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetUnderlyingRsrcItr());

				    //    cur_pass->InsertSameQueueSyncInfo(
				    //        cur_pass,
				    //        later_pass,
				    //        cur_pass_queue_info.first->GetQueueCap(),
				    //        providing_outlet.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetRsrcUsage()->GetSynInfo(),
				    //        later_let.GetUnderlyingRsrcItr());
				    //   }
				    //  }
				    // }
				    // //*********************************
				    // //CASE 2: NOT in a same queue situation (need queue ownership transfer!)
				    //}
			    }
		    }

		);
	}

	//处理多个pass 同时 读 的同步问题

	//生成SSIS
	//auto ssis_size = queue_with_all_passes_on_it.size();

	//std::for_each(topologically_sorted_nodes.begin(), topologically_sorted_nodes.end(), [&sufficient_synchronization_index_set, ssis_size](PassNode *ptr_pass) {
	//	sufficient_synchronization_index_set.emplace(ptr_pass, std::array<std::vector<size_t>, 2>{std::vector<size_t>(ssis_size, 0), std::vector<size_t>(ssis_size, 0)});
	//});

	////填写queue_with_depen_passes_on_it
	//std::for_each(topologically_sorted_nodes.begin(), topologically_sorted_nodes.end(),
	//              [&pass_on_queue_info, &queue_with_all_passes_on_it](PassNode *ptr_pass) {
	//	              std::unordered_map<VkDeviceManager::VkExecutionQueue *, std::vector<PassNode *>> queue_with_depen_passes_on_it;        //每一个pass的depen pass都需要一个queue_with_depen_passes_on_it

	//	              //loop 所有 之前需要先执行的depen pass，从而填充 queue_with_depen_passes_on_it
	//	              std::for_each(ptr_pass->passes_depen_set.begin(), ptr_pass->passes_depen_set.end(),
	//	                            [&pass_on_queue_info, &queue_with_all_passes_on_it, &queue_with_depen_passes_on_it](PassNode *pas) {
	//		                            //找到先执行pass的queue，如果在，就加入，不在就创建新的加入
	//		                            auto ptr_to_queue = pass_on_queue_info.at(pas).first;
	//		                            if (!queue_with_depen_passes_on_it.contains(ptr_to_queue))
	//		                            {
	//			                            queue_with_depen_passes_on_it.emplace(ptr_to_queue, std::vector<PassNode *>{pas});
	//		                            }
	//		                            else
	//		                            {
	//			                            queue_with_depen_passes_on_it.at(ptr_to_queue).push_back(pas);
	//		                            }
	//	                            });

	//	              //按照在某一个queue内，按照assigned index排序所有需要先执行的depen pass
	//	              std::for_each(queue_with_depen_passes_on_it.begin(), queue_with_depen_passes_on_it.end(),
	//	                            [&pass_on_queue_info](std::pair<VkDeviceManager::VkExecutionQueue *, std::vector<PassNode *>> &queue_passlist_pair) {
	//		                            std::sort(queue_passlist_pair.second.begin(), queue_passlist_pair.second.end(),
	//		                                      [&pass_on_queue_info](PassNode *lhs, PassNode *rhs) {
	//			                                      pass_on_queue_info.at(lhs).second < pass_on_queue_info.at(rhs).second;
	//		                                      });
	//	                            });
	//	              //对于每一个depen pass：
	//	              std::for_each(ptr_pass->passes_depen_set.begin(), ptr_pass->passes_depen_set.end(),

	//	              )
	//              })

	//
	//std::unordered_map<VkDeviceManager::VkExecutionQueue *, std::list<PassNode *>>         queue_with_passes_on_it;
	//std::unordered_map<PassNode *, std::pair<VkDeviceManager::VkExecutionQueue *, size_t>> pass_on_queue_info;
	//std::unordered_map<VkDeviceManager::VkExecutionQueue *, std::vector<PassNode *>>       queue_with_depen_passes_on_it;
	//std::unordered_map<PassNode *, std::vector<size_t>>                                    sufficient_synchronization_index_set;

	/// <summary>
	/// 生成subgraph
	/// </summary>
	/// <returns></returns>
	//for (const auto &gfx_node : gfx_pass_nodes)
	//{
	//	if (gfx_node->IsRootPass())
	//	{
	//		//Root pass marking
	//		RootPassMarking(gfx_node.get());
	//		//sub graph overlap testing
	//		SubGraphMerge();
	//	}
	//}

	//for (const auto &gfx_node : gfx_pass_nodes)
	//{
	//	if (gfx_node->IsRootPass())
	//	{
	//		gfx_node->AccumulateQueueRequirement();
	//	}
	//}

	std::cout << "Render graph compilation finished." << std::endl;
	return true;
}

void RenderGraphV0::DependencyGraph::ExecutePass(VkCommandBuffer cmd_buf, PassNode *pass)
{
	//REQUIRED RESOURCES INITIALIZATION
	for (auto &buf : pass->buf_init_list)
	{
		//VirtualResource has a descriptor to help initalize resources
		buf->second.Actualize(renderpass_manager);
	}

	for (auto &tex : pass->uni_tex_init_list)
	{
		//VirtualResource has a descriptor to help initalize resources
		tex->second.Actualize(renderpass_manager);
	}

	//BARRIERS INSERSION before PASS EXECUTION
	for (auto &[source_syn, target_syn, resource_itor] : pass->buf_syn_infos)
	{
		//不同资源insert barrier的方法不同，这里可以根据类型做特化；
		resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
	}

	for (auto &[source_syn, target_syn, resource_itor] : pass->uni_tex_syn_infos)
	{
		//不同资源insert barrier的方法不同，这里可以根据类型做特化；
		resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
	}

	//RENDERPASS INITIALIZATION AND EXECUTION
	pass->PreExecute();
	pass->Execute(cmd_buf);

	//TRANSIENT RESOURCES DESTRUCTION
	for (const auto &rsrc_buf : pass->buf_recycle_list)
	{
		rsrc_buf->second.Recycle(renderpass_manager);
	}

	for (const auto &rsrc_tex : pass->uni_tex_recycle_list)
	{
		rsrc_tex->second.Recycle(renderpass_manager);
	}

	////TODO: BARRIERS INSERSION after Pass？也许还需要在pass之后插入barrier？
}

void RenderGraphV0::DependencyGraph::RootPassMarking(PassNode *root_pass)
{
	std::unordered_set<PassNode *> pass_set{root_pass};
	std::unordered_set<PassNode *> removed_pass_set{};

	while (!pass_set.empty())
	{
		const auto curr_pass = *pass_set.begin();
		if (!removed_pass_set.contains(curr_pass))
		{
			for (auto &inlet : curr_pass->ins_buf)
			{
				const auto usage     = inlet.second.GetRsrcUsage();
				auto &     buf_usage = *usage;

				if (buf_usage.GetAccessType() == Vk::AccessType::Read)
				{
					const auto rsrc_itr = inlet.second.GetUnderlyingRsrcItr();
					//根据确定的顺序（至少用户使用In和Out函数 所声明的资源使用顺序是有保证的）进行排序
					std::sort(rsrc_itr->second.passes_access_this_rsrc.begin(), rsrc_itr->second.passes_access_this_rsrc.end(),
					          [&](const std::pair<RenderGraphV0::PassNode *, Vk::AccessType> &a, const std::pair<RenderGraphV0::PassNode *, Vk::AccessType> &b) {
						          return CompareByFinalExeOrder(a.first, b.first, final_execution_order_hint);
					          });
					//找到最后一个写入的pass
					for (auto access_pass = rsrc_itr->second.passes_access_this_rsrc.rbegin(); access_pass != rsrc_itr->second.passes_access_this_rsrc.rend(); ++access_pass)
					{
						if (access_pass->second == Vk::AccessType::Write)
						{
							//tag root pass
							access_pass->first->TagCurRootPass(root_pass);
							//加入到豪华午餐
							pass_set.insert(access_pass->first);
							root_pass->AddSubGraphPass(access_pass->first);
							break;
						}
					}
				}
			}

			for (auto &inlet : curr_pass->ins_uni_tex)
			{
				const auto usage     = inlet.second.GetRsrcUsage();
				auto &     tex_usage = *usage;

				if (tex_usage.GetAccessType() == Vk::AccessType::Read)
				{
					const auto rsrc_itr = inlet.second.GetUnderlyingRsrcItr();
					//根据确定的顺序（至少用户使用In和Out函数 所声明的资源使用顺序是有保证的）进行排序
					std::sort(rsrc_itr->second.passes_access_this_rsrc.begin(), rsrc_itr->second.passes_access_this_rsrc.end(),
					          [&](const std::pair<RenderGraphV0::PassNode *, Vk::AccessType> &a, const std::pair<RenderGraphV0::PassNode *, Vk::AccessType> &b) {
						          return CompareByFinalExeOrder(a.first, b.first, final_execution_order_hint);
					          });

					//找到最后一个写入的pass
					for (auto access_pass = rsrc_itr->second.passes_access_this_rsrc.rbegin(); access_pass != rsrc_itr->second.passes_access_this_rsrc.rend(); ++access_pass)
					{
						if (access_pass->second == Vk::AccessType::Write)
						{
							//tag root pass
							access_pass->first->TagCurRootPass(root_pass);
							//加入到豪华午餐
							pass_set.insert(access_pass->first);
							root_pass->AddSubGraphPass(access_pass->first);
							break;
						}
					}
				}
			}

			pass_set.erase(curr_pass);
			removed_pass_set.insert(curr_pass);
		}
	}

	//对subgraph排序
	std::sort(root_pass->subgraph_pass.begin(), root_pass->subgraph_pass.end(), [&](const RenderGraphV0::PassNode *a, const RenderGraphV0::PassNode *b) {
		return CompareByFinalExeOrder(a, b, final_execution_order_hint);
	});
}

void RenderGraphV0::DependencyGraph::SubGraphMerge()
{
	//For passes to be absolutely free of resource contention, we must guarantee that there never is any resource which is read by one subgraph, and written by another subgraph. If all sub-graphs just READ a resource, that’s fine.

	//Merge Subgraph
	for (const auto &buf : buffers_map)
	{
		std::unordered_set<PassNode *> read_set;
		std::unordered_set<PassNode *> write_set;
		for (const auto &pass_and_accesstype : buf.second.passes_access_this_rsrc)
		{
			if (pass_and_accesstype.second == Vk::AccessType::Read)
			{
				read_set.insert(pass_and_accesstype.first);
			}
			else
			{
				write_set.insert(pass_and_accesstype.first);
			}
		}

		for (const auto write_pas : write_set)
		{
			for (const auto read_pas : read_set)
			{
				if (write_pas->root_pass_tag != read_pas->root_pass_tag)
				{
					//merge subgraph
					for (const auto &pas : read_pas->root_pass_tag->subgraph_pass)
					{
						pas->root_pass_tag = write_pas->root_pass_tag;
					}

					read_pas->root_pass_tag->SetAsNonRootPass();
					read_pas->root_pass_tag->root_pass_tag = write_pas->root_pass_tag;
					write_pas->root_pass_tag->subgraph_pass.insert(read_pas->root_pass_tag->subgraph_pass.begin(), read_pas->root_pass_tag->subgraph_pass.end());
					read_pas->root_pass_tag->subgraph_pass.clear();
				}
			}
		}
	}

	for (const auto &tex : uni_textures_map)
	{
		std::unordered_set<PassNode *> read_set;
		std::unordered_set<PassNode *> write_set;
		for (const auto &pass_and_accesstype : tex.second.passes_access_this_rsrc)
		{
			if (pass_and_accesstype.second == Vk::AccessType::Read)
			{
				read_set.insert(pass_and_accesstype.first);
			}
			else
			{
				write_set.insert(pass_and_accesstype.first);
			}
		}

		for (const auto write_pas : write_set)
		{
			for (const auto read_pas : read_set)
			{
				if (write_pas->root_pass_tag != read_pas->root_pass_tag)
				{
					//merge subgraph
					for (const auto &pas : read_pas->root_pass_tag->subgraph_pass)
					{
						pas->root_pass_tag = write_pas->root_pass_tag;
					}

					read_pas->root_pass_tag->SetAsNonRootPass();
					read_pas->root_pass_tag->root_pass_tag = write_pas->root_pass_tag;
					write_pas->root_pass_tag->subgraph_pass.insert(read_pas->root_pass_tag->subgraph_pass.begin(), read_pas->root_pass_tag->subgraph_pass.end());
					read_pas->root_pass_tag->subgraph_pass.clear();
				}
			}
		}
	}

	//Record in Resource the Subgraph Sharing Condition
	for (auto &buf : buffers_map)
	{
		std::unordered_set<PassNode *> read_set;
		std::unordered_set<PassNode *> write_set;
		for (const auto &pass_and_accesstype : buf.second.passes_access_this_rsrc)
		{
			if (pass_and_accesstype.second == Vk::AccessType::Read)
			{
				read_set.insert(pass_and_accesstype.first);
			}
			else
			{
				write_set.insert(pass_and_accesstype.first);
			}
		}

		using ReadPass = PassNode;
		using RootPass = PassNode;
		std::unordered_multimap<RootPass *, ReadPass *> root_read_passes_map;

		for (const auto read_pas : read_set)
		{
			root_read_passes_map.emplace(read_pas->root_pass_tag, read_pas);
		}

		buf.second.AssignReadPassesMap(std::move(root_read_passes_map));

		//之后如何找到所有？：
		// std::unordered_multimap<int, std::string> myMap;

		//   // Insert some key-value pairs into the unordered_multimap
		//   myMap.emplace(1, "apple");
		//   myMap.emplace(2, "banana");
		//   myMap.emplace(1, "orange");
		//   myMap.emplace(3, "grape");

		//   int key = 1;

		//   // Get the range of elements with the specified key
		//   auto range = myMap.equal_range(key);

		//   // Iterate over the range and print the associated values
		//   for (auto it = range.first; it != range.second; ++it) {
		//       std::cout << it->second << std::endl;
	}

	//Record in Resource the Subgraph Sharing Condition
	for (auto &tex : uni_textures_map)
	{
		std::unordered_set<PassNode *> read_set;
		std::unordered_set<PassNode *> write_set;
		for (const auto &pass_and_accesstype : tex.second.passes_access_this_rsrc)
		{
			if (pass_and_accesstype.second == Vk::AccessType::Read)
			{
				read_set.insert(pass_and_accesstype.first);
			}
			else
			{
				write_set.insert(pass_and_accesstype.first);
			}
		}

		using ReadPass = PassNode;
		using RootPass = PassNode;
		std::unordered_multimap<RootPass *, ReadPass *> root_read_passes_map;

		for (const auto read_pas : read_set)
		{
			root_read_passes_map.emplace(read_pas->root_pass_tag, read_pas);
		}

		tex.second.AssignReadPassesMap(std::move(root_read_passes_map));
		//之后如何找到所有？：
		// std::unordered_multimap<int, std::string> myMap;

		//   // Insert some key-value pairs into the unordered_multimap
		//   myMap.emplace(1, "apple");
		//   myMap.emplace(2, "banana");
		//   myMap.emplace(1, "orange");
		//   myMap.emplace(3, "grape");

		//   int key = 1;

		//   // Get the range of elements with the specified key
		//   auto range = myMap.equal_range(key);

		//   // Iterate over the range and print the associated values
		//   for (auto it = range.first; it != range.second; ++it) {
		//       std::cout << it->second << std::endl;
	}
}

bool RenderGraphV0::DependencyGraph::CompareByFinalExeOrder(const PassNode *a, const PassNode *b, const std::vector<PassNode *> &order)
{
	// Find the indices of a and b in the order vector
	auto itA = std::find(order.begin(), order.end(), a);
	auto itB = std::find(order.begin(), order.end(), b);

	// If a or b is not found in the order vector, they are considered equal
	if (itA == order.end() || itB == order.end())
	{
		return false;
	}

	// Compare the indices of a and b in the order vector
	return std::distance(order.begin(), itA) < std::distance(order.begin(), itB);
}

bool RenderGraphV0::DependencyGraph::ExecutRenderGraphV0(VkCommandBuffer cmb)
{
	//using namespace std;

	////资源初始化信息 已经记录在attch_init_list，texture_init_list，buffer_init_list当中了，
	////资源销毁信息 和 同步信息录入

	////资源销毁信息录入
	//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	//{
	//	if (buf_itr->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, buf_itr->second.passes_access_this_rsrc);
	//		last_pass->buf_recycle_list.push_back(buf_itr);
	//	}
	//}

	//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	//{
	//	if (tex_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, tex_itr->second.passes_access_this_rsrc);
	//		last_pass->tex_recycle_list.push_back(tex_itr);
	//	}
	//}

	//for (auto attch_itr = attachments_map.begin(); attch_itr != attachments_map.end(); ++attch_itr)
	//{
	//	if (attch_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, attch_itr->second.passes_access_this_rsrc);
	//		last_pass->attch_recycle_list.push_back(attch_itr);
	//	}
	//}

	////同步信息录入这里sync info可以根据资源类型采取不同处理（资源的其他状态转换？比如sampler，view等，怎么处理？）

	////BUF处理
	//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	//{
	//	SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = buf_itr->second.name;

	//	for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

	//			auto outlet_itr = fir_pass->outs_buf.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_buf.end())
	//			{
	//				auto rsrc_usage = outlet_itr->second.GetRsrcUsage();        //所有usages
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				//第一个使用方式拿出syncinfo
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
	//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second.GetItrInRsrcMap());
	//			}

	//			auto inlet_itr = fir_pass->ins_buf.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_buf.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second.GetItrInRsrcMap());
	//			}
	//		}

	//		//如果第二个pass不是最后一个pass（存在至少两个pass）
	//		if (sec_pass_itr != buf_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//同时有第二个pass和第一个pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");

	//			if (fir_pass->outs_buf.contains(outlet_name_src_fir))
	//			{
	//				const auto temp_itr       = fir_pass->outs_buf.find(outlet_name_src_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}
	//			if (fir_pass->ins_buf.contains(inlet_name_tar_fir))
	//			{
	//				const auto temp_itr       = fir_pass->ins_buf.find(inlet_name_tar_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                     outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
	//			auto                                                                     inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
	//			std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_buf.contains(outlet_name_src_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->outs_buf.find(outlet_name_src_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}
	//			if (sec_pass->ins_buf.contains(inlet_name_tar_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->ins_buf.find(inlet_name_tar_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}
	////TEX处理
	//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	//{
	//	SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = tex_itr->second.name;

	//	for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

	//			auto outlet_itr_tex = fir_pass->outs_tex.find(outlet_name);
	//			if (outlet_itr_tex != fir_pass->outs_tex.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr_tex->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				//从第一个使用方式拿出syncinfo
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());
	//			}

	//			auto outlet_itr_atta = fir_pass->outs_attach.find(outlet_name);
	//			if (outlet_itr_atta != fir_pass->outs_attach.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr_atta->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				//从第一个使用方式拿出syncinfo
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();
	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());
	//			}

	//			auto inlet_itr_tex = fir_pass->ins_tex.find(inlet_name);
	//			if (inlet_itr_tex != fir_pass->ins_tex.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr_tex->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//从第一个使用方式拿出syncinfo
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());
	//			}

	//			auto inlet_itr_atta = fir_pass->ins_tex.find(inlet_name);
	//			if (inlet_itr_atta != fir_pass->ins_tex.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr_atta->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//从第一个使用方式拿出syncinfo
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());
	//			}
	//		}

	//		//如果第二个pass不是最后一个pass（存在至少两个pass）
	//		if (sec_pass_itr != tex_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//同时有第二个pass和第一个pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");

	//			if (fir_pass->outs_tex.contains(outlet_name_src_fir))
	//			{
	//				const auto temp_itr       = fir_pass->outs_tex.find(outlet_name_src_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}
	//			if (fir_pass->ins_tex.contains(inlet_name_tar_fir))
	//			{
	//				const auto temp_itr       = fir_pass->ins_tex.find(outlet_name_src_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}

	//			if (fir_pass->outs_attach.contains(outlet_name_src_fir))
	//			{
	//				const auto temp_itr       = fir_pass->outs_attach.find(outlet_name_src_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}
	//			if (fir_pass->ins_attach.contains(inlet_name_tar_fir))
	//			{
	//				const auto temp_itr       = fir_pass->ins_attach.find(outlet_name_src_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
	//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_tex.contains(outlet_name_src_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->outs_tex.find(outlet_name_src_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}
	//			if (sec_pass->ins_tex.contains(inlet_name_tar_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->ins_tex.find(inlet_name_tar_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}

	//			if (sec_pass->outs_attach.contains(outlet_name_src_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->outs_attach.find(outlet_name_src_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}
	//			if (sec_pass->ins_attach.contains(inlet_name_tar_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->ins_attach.find(inlet_name_tar_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}

	////ATTACHMENT处理   (同步信息+INFERENCE of the loading operation of ATTACHMENTs  推导attachment资源的 加载load 和 存储store 操作)
	//for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
	//{
	//	SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = attach_itr->second.name;

	//	for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

	//			auto outlet_itr_atta = fir_pass->outs_attach.find(outlet_name);
	//			if (outlet_itr_atta != fir_pass->outs_attach.end())
	//			{
	//				const auto rsrc_usage = outlet_itr_atta->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());

	//				//因为是第一个使用的pass，目前attachment里面还没有内容
	//				rsrc_usage->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//因为使用的是Out函数导出，所以要用store operation
	//				rsrc_usage->store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			auto outlet_itr_tex = fir_pass->outs_tex.find(outlet_name);
	//			if (outlet_itr_tex != fir_pass->outs_tex.end())
	//			{
	//				const auto rsrc_usage = outlet_itr_tex->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());

	//				////因为是第一个使用的pass，目前attachment里面还没有内容
	//				//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				////因为使用的是Out函数导出，所以要用store operation
	//				//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			auto inlet_itr_atta = fir_pass->ins_attach.find(inlet_name);
	//			if (inlet_itr_atta != fir_pass->ins_attach.end())
	//			{
	//				const auto rsrc_usage = inlet_itr_atta->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());

	//				//因为是第一个使用的pass，目前attachment里面还没有内容
	//				rsrc_usage->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//因为使用的是In函数，不会继续导出，所以要用Don't care
	//				rsrc_usage->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//			}

	//			auto inlet_itr_tex = fir_pass->ins_tex.find(inlet_name);
	//			if (inlet_itr_tex != fir_pass->ins_tex.end())
	//			{
	//				const auto rsrc_usage = inlet_itr_tex->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage->GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());

	//				//因为是第一个使用的pass，目前attachment里面还没有内容
	//				//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//因为使用的是In函数，不会继续导出，所以要用Don't care
	//				//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//			}
	//		}

	//		if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//同时有第二个pass和第一个pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");

	//			if (fir_pass->outs_attach.contains(outlet_name_src_fir))
	//			{
	//				const auto temp_itr       = fir_pass->outs_attach.find(outlet_name_src_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}
	//			if (fir_pass->ins_attach.contains(inlet_name_tar_fir))
	//			{
	//				const auto temp_itr       = fir_pass->ins_attach.find(inlet_name_tar_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}

	//			if (fir_pass->outs_tex.contains(outlet_name_src_fir))
	//			{
	//				const auto temp_itr       = fir_pass->outs_tex.find(outlet_name_src_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}
	//			if (fir_pass->ins_tex.contains(inlet_name_tar_fir))
	//			{
	//				const auto temp_itr       = fir_pass->ins_tex.find(inlet_name_tar_fir);
	//				auto       rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source           = rsrc_usage_src->GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
	//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_attach.contains(outlet_name_src_sec))
	//			{
	//				const auto temp_itr       = sec_pass->outs_attach.find(outlet_name_src_sec);
	//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				rsrc_usage_tar->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				rsrc_usage_tar->store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}
	//			if (sec_pass->ins_attach.contains(inlet_name_tar_sec))
	//			{
	//				const auto temp_itr       = sec_pass->ins_attach.find(inlet_name_tar_sec);
	//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				//读写后不会导出，所以用dont care？存疑
	//				rsrc_usage_tar->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				rsrc_usage_tar->store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			if (sec_pass->outs_tex.contains(outlet_name_src_sec))
	//			{
	//				const auto temp_itr       = sec_pass->outs_tex.find(outlet_name_src_sec);
	//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				//(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				//(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}
	//			if (sec_pass->ins_tex.contains(inlet_name_tar_sec))
	//			{
	//				const auto temp_itr       = sec_pass->ins_tex.find(inlet_name_tar_sec);
	//				auto       rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar->GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				//读写后不会导出，所以用dont care？存疑
	//				//(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				//(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}

	//PASSES EXECUTION(TODO:ORGANIZE PASSES INTO GROUPS FOR PARALLEL EXECUTION)

	for (const auto pass_node_ptr : final_execution_order_hint)
	{
		ExecutePass(cmb, pass_node_ptr);
	}

	std::cout << "Render graph command recording finished" << std::endl;
	return true;
}

bool RenderGraphV0::DependencyGraph::ParallelExecuteRenderGraphV0(const VkDeviceManager &device_man, VkSemaphore general_rendering_finished_semaphore, VkSemaphore image_available_semaphore)
{
	std::vector<std::pair<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::VkExecutionQueue *>> subgraph_queue_pairs;
	std::pair<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::VkExecutionQueue *>              first_gfx_subgraph_queue_pair;
	bool                                                                                           first_gfx_subgraph_processed = false;

	std::unordered_map<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> subgraph_execute_on_queue_info;
	using OnQueueOrder = size_t;
	std::unordered_map<RenderGraphV0::GraphicsPassNode *, OnQueueOrder> subgraph_execute_on_queue_order;

	device_man.QueuesNumDisbatchedPassesToZero();

	//Pre Execute
	for (auto &pass : gfx_pass_nodes)
	{
		OnQueueOrder multi_queues_order = 0;
		if (pass->is_rootpass)
		{
			multi_queues_order++;
			VkRenderpassBaseRG::Type         acc_type = pass->GetAccumulatedType();
			VkDeviceManager::QueueCapability queue_cap;
			if (acc_type & VkRenderpassBaseRG::Type::Graphics)
			{
				queue_cap.graphics = true;
			}

			if (acc_type & VkRenderpassBaseRG::Type::Compute)
			{
				queue_cap.compute = true;
			}

			if (acc_type & VkRenderpassBaseRG::Type::Transfor)
			{
				queue_cap.transfer = true;
			}

			if (acc_type & VkRenderpassBaseRG::Type::Present)
			{
				queue_cap.present = true;
			}

			auto &target_queue = device_man.GetSuitableQueue(queue_cap);
			target_queue.SetNumDisbatchedPasses(target_queue.GetNumDisbatchedPasses() + pass->subgraph_pass.size() + 1);

			auto insersion_result_0 = subgraph_execute_on_queue_info.try_emplace(pass.get(), target_queue.GetQueueCap());
			auto insersion_result_1 = subgraph_execute_on_queue_order.try_emplace(pass.get(), multi_queues_order);

			if (!insersion_result_0.second || !insersion_result_1.second)
			{
				throw std::runtime_error("One subgraph can't be executed twice!");
			}

			//TODO: parallel recording cmd buf
			//VkCommandBuffer cmd_buf = target_queue.GetOneNewBufFromCurQueue();
			//pass->Execute(cmd_buf);
			//for (auto subgraph_pass : pass->subgraph_pass)
			//{
			//	subgraph_pass->Execute(cmd_buf);
			//}
			//parallel recording done

			//if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && first_gfx_subgraph_processed)
			//{
			//	target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf, image_available_semaphore);
			//	first_gfx_subgraph_queue_pair.first  = pass.get();
			//	first_gfx_subgraph_queue_pair.second = &target_queue;
			//	first_gfx_subgraph_processed         = true;
			//}
			//else if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && !first_gfx_subgraph_processed)
			//{
			//	auto timeline_sem = first_gfx_subgraph_queue_pair.second->GetTimeLineSem();
			//	target_queue.SubmitCmdBufThenSignalTimeLineSemThenWaitGfxTimeLineSem(cmd_buf, timeline_sem);

			//	subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
			//}
			//else
			//{
			//	target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf);
			//	subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
			//}
		}
	}

	//DO IT AGAIN
	device_man.QueuesNumDisbatchedPassesToZero();
	for (auto &pass : gfx_pass_nodes)
	{
		OnQueueOrder on_queue_order = 0;
		if (pass->is_rootpass)
		{
			on_queue_order++;
			VkRenderpassBaseRG::Type         acc_type = pass->GetAccumulatedType();
			VkDeviceManager::QueueCapability queue_cap;
			if (acc_type & VkRenderpassBaseRG::Type::Graphics)
			{
				queue_cap.graphics = true;
			}

			if (acc_type & VkRenderpassBaseRG::Type::Compute)
			{
				queue_cap.compute = true;
			}

			if (acc_type & VkRenderpassBaseRG::Type::Transfor)
			{
				queue_cap.transfer = true;
			}

			if (acc_type & VkRenderpassBaseRG::Type::Present)
			{
				queue_cap.present = true;
			}
			auto &target_queue = device_man.GetSuitableQueue(queue_cap);
			target_queue.SetNumDisbatchedPasses(target_queue.GetNumDisbatchedPasses() + pass->subgraph_pass.size() + 1);

			//TODO: PARALLEL RECORDING MULTI CMD BUF
			VkCommandBuffer cmd_buf = target_queue.GetOneNewBufFromCurQueue();
			pass->Execute(cmd_buf);
			for (auto subgraph_pass : pass->subgraph_pass)
			{
				subgraph_pass->Execute(cmd_buf);
			}
			//PARALLEL RECORDING DONE

			if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && first_gfx_subgraph_processed)
			{
				target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf, image_available_semaphore);
				first_gfx_subgraph_queue_pair.first  = pass.get();
				first_gfx_subgraph_queue_pair.second = &target_queue;
				first_gfx_subgraph_processed         = true;
			}
			else if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && !first_gfx_subgraph_processed)
			{
				auto timeline_sem = first_gfx_subgraph_queue_pair.second->GetTimeLineSem();
				target_queue.SubmitCmdBufThenSignalTimeLineSemThenWaitGfxTimeLineSem(cmd_buf, timeline_sem);

				subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
			}
			else
			{
				target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf);
				subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
			}
		}
	}

	if (!first_gfx_subgraph_queue_pair.first || !first_gfx_subgraph_queue_pair.second)
	{
		throw std::runtime_error("No graphics pass provided!");
	}

	//harvests all Timeline Semaphores across all queues
	std::vector<VkSemaphore> all_waiting_queue_sems;
	for (auto &queue_pair : subgraph_queue_pairs)
	{
		all_waiting_queue_sems.push_back(queue_pair.second->GetTimeLineSem());
	}

	first_gfx_subgraph_queue_pair.second->Finalize(all_waiting_queue_sems);

	device_man.QueuesTimeLineSemToZero();

	return true;
}

bool RenderGraphV0::DependencyGraph::ParallelExecuteRenderGraphV0V0(const VkDeviceManager &device_man, VkSemaphore general_rendering_finished_semaphore, VkSemaphore image_available_semaphore)
{
	//device_man.QueuesNumDisbatchedPassesToZero();

	//Pre Execute
	//for (auto &pass : gfx_pass_nodes)
	//{
	//	OnQueueOrder multi_queues_order = 0;
	//	if (pass->is_rootpass)
	//	{
	//		multi_queues_order++;
	//		VkRenderpassBaseRG::Type         acc_type = pass->GetAccumulatedType();
	//		VkDeviceManager::QueueCapability queue_cap;
	//		if (acc_type & VkRenderpassBaseRG::Type::Graphics)
	//		{
	//			queue_cap.graphics = true;
	//		}

	//		if (acc_type & VkRenderpassBaseRG::Type::Compute)
	//		{
	//			queue_cap.compute = true;
	//		}

	//		if (acc_type & VkRenderpassBaseRG::Type::Transfor)
	//		{
	//			queue_cap.transfer = true;
	//		}

	//		if (acc_type & VkRenderpassBaseRG::Type::Present)
	//		{
	//			queue_cap.present = true;
	//		}

	//		auto &target_queue = device_man.GetSuitableQueue(queue_cap);
	//		target_queue.SetNumDisbatchedPasses(target_queue.GetNumDisbatchedPasses() + pass->subgraph_pass.size() + 1);

	//		auto insersion_result_0 = subgraph_execute_on_queue_info.try_emplace(pass.get(), target_queue.GetQueueCap());
	//		auto insersion_result_1 = subgraph_execute_on_queue_order.try_emplace(pass.get(), multi_queues_order);

	//		if (!insersion_result_0.second || !insersion_result_1.second)
	//		{
	//			throw std::runtime_error("One subgraph can't be executed twice!");
	//		}

	//		//TODO: parallel recording cmd buf
	//		//VkCommandBuffer cmd_buf = target_queue.GetOneNewBufFromCurQueue();
	//		//pass->Execute(cmd_buf);
	//		//for (auto subgraph_pass : pass->subgraph_pass)
	//		//{
	//		//	subgraph_pass->Execute(cmd_buf);
	//		//}
	//		//parallel recording done

	//		//if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && first_gfx_subgraph_processed)
	//		//{
	//		//	target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf, image_available_semaphore);
	//		//	first_gfx_subgraph_queue_pair.first  = pass.get();
	//		//	first_gfx_subgraph_queue_pair.second = &target_queue;
	//		//	first_gfx_subgraph_processed         = true;
	//		//}
	//		//else if ((acc_type & VkRenderpassBaseRG::Type::Graphics) && !first_gfx_subgraph_processed)
	//		//{
	//		//	auto timeline_sem = first_gfx_subgraph_queue_pair.second->GetTimeLineSem();
	//		//	target_queue.SubmitCmdBufThenSignalTimeLineSemThenWaitGfxTimeLineSem(cmd_buf, timeline_sem);

	//		//	subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
	//		//}
	//		//else
	//		//{
	//		//	target_queue.SubmitCmdBufThenSignalTimeLineSem(cmd_buf);
	//		//	subgraph_queue_pairs.emplace_back(pass.get(), &target_queue);
	//		//}
	//	}
	//}

	//TODO: PARALLEL RECORDING MULTI CMD BUF. Should be done in a per-queue basis? You can't fill command buffers created by the same pool from multi threads
	//在同一level中所有的pass的录制顺序并不重要
	for (auto level = 0; level <= max_level; ++level)
	{
		for (auto &ptr_pass : topologically_sorted_nodes)
		{
			if (ptr_pass->maximum_recursion_depth == level)
			{
				auto            ptr_queue = pass_on_queue_info.at(ptr_pass).first;
				VkCommandBuffer cmd_buf   = ptr_queue->GetOneNewBufFromCurQueue();
				ptr_pass->PreExecute();
				ptr_pass->Execute(cmd_buf);
			}
		}
	}

	//harvests all Timeline Semaphores across all queues
	std::vector<VkSemaphore> all_waiting_queue_sems;
	for (auto &[queue_ptr, _] : queue_with_all_passes_on_it)
	{
		all_waiting_queue_sems.push_back(queue_ptr->GetTimeLineSem());
	}

	//first_gfx_subgraph_queue_pair.second->Finalize(all_waiting_queue_sems);

	return true;
}

bool RenderGraphV0::DependencyGraph::Execute(VkCommandBuffer cmb)
{
	//using namespace std;

	////资源初始化信息 已经记录在attch_init_list，texture_init_list，buffer_init_list当中了，
	////资源销毁信息 和 同步信息录入

	////资源销毁信息录入
	//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	//{
	//	if (buf_itr->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, buf_itr->second.passes_access_this_rsrc);
	//		last_pass->buf_recycle_list.push_back(buf_itr);
	//	}
	//}

	//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	//{
	//	if (tex_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, tex_itr->second.passes_access_this_rsrc);
	//		last_pass->tex_recycle_list.push_back(tex_itr);
	//	}
	//}

	//for (auto attch_itr = attachments_map.begin(); attch_itr != attachments_map.end(); ++attch_itr)
	//{
	//	if (attch_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, attch_itr->second.passes_access_this_rsrc);
	//		last_pass->attch_recycle_list.push_back(attch_itr);
	//	}
	//}

	////for ( auto attach_itr = attachments_map.begin();attach_itr != attachments_map.end();++attach_itr)
	////{
	////	const auto original_rsrc_name = attach_itr->second.name;

	////	for (auto fir_pass_itr =  attach_itr->second.passes_access_this_rsrc.begin();fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	////	{
	////		//  根据attach 会作为 inlet 输入pass：
	////		//或 根据attach 会作为 outlet 输入pass：

	////		const auto first_pass  = *fir_pass_itr;
	////		auto outlet_name = original_rsrc_name + first_pass->name + std::string("Out");
	////		auto inlet_name  = original_rsrc_name + first_pass->name + std::string("In");

	////		//第一个pass特殊处理
	////		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
	////		{
	////

	////		}

	////	}

	////}

	////同步信息录入这里sync info可以根据资源类型采取不同处理（资源状态转换？比如sampler，view等，怎么处理？）
	////buf处理
	//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	//{
	//	SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = buf_itr->second.name;

	//	for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_buf.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_buf.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
	//			}

	//			auto inlet_itr = fir_pass->ins_buf.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_buf.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);
	//			}
	//		}

	//		if (sec_pass_itr != buf_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//同时有第二个pass和第一个pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");

	//			if (fir_pass->outs_buf.contains(outlet_name_src_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->outs_buf.find(outlet_name_src_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}
	//			if (fir_pass->ins_buf.contains(inlet_name_tar_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->ins_buf.find(inlet_name_tar_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                     outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
	//			auto                                                                     inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
	//			std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_buf.contains(outlet_name_src_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->outs_buf.find(outlet_name_src_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}
	//			if (sec_pass->ins_buf.contains(inlet_name_tar_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->ins_buf.find(inlet_name_tar_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}
	////tex处理
	//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	//{
	//	SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = tex_itr->second.name;

	//	for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_tex.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_tex.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
	//			}

	//			auto inlet_itr = fir_pass->ins_tex.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_tex.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);
	//			}
	//		}

	//		if (sec_pass_itr != tex_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//同时有第二个pass和第一个pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");

	//			if (fir_pass->outs_tex.contains(outlet_name_src_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->outs_tex.find(outlet_name_src_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}
	//			if (fir_pass->ins_tex.contains(inlet_name_tar_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->ins_tex.find(inlet_name_tar_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
	//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_tex.contains(outlet_name_src_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->outs_tex.find(outlet_name_src_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}
	//			if (sec_pass->ins_tex.contains(inlet_name_tar_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->ins_tex.find(inlet_name_tar_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}
	////attachmen处理(同步信息+INFERENCE of the loading operation of ATTACHMENTs  推导attachment资源的 加载load 和 存储store 操作)
	//for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
	//{
	//	SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = attach_itr->second.name;

	//	for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_attach.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_attach.end())
	//			{
	//				auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);

	//				//因为是第一个使用的pass，目前attachment里面还没有内容
	//				rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//因为使用的是Out函数导出，所以要用store operation
	//				rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			auto inlet_itr = fir_pass->ins_attach.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_attach.end())
	//			{
	//				auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);

	//				//因为是第一个使用的pass，目前attachment里面还没有内容
	//				rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//因为使用的是In函数，不会导出，所以要用Don't care
	//				rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//			}
	//		}

	//		if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//同时有第二个pass和第一个pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");

	//			if (fir_pass->outs_attach.contains(outlet_name_src_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->outs_attach.find(outlet_name_src_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}
	//			if (fir_pass->ins_attach.contains(inlet_name_tar_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->ins_attach.find(inlet_name_tar_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
	//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_attach.contains(outlet_name_src_sec))
	//			{
	//				const auto temp_itr       = sec_pass->outs_attach.find(outlet_name_src_sec);
	//				auto &     rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				rsrc_usage_tar.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				rsrc_usage_tar.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}
	//			if (sec_pass->ins_attach.contains(inlet_name_tar_sec))
	//			{
	//				const auto temp_itr       = sec_pass->ins_attach.find(inlet_name_tar_sec);
	//				auto &     rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				//读写后不会导出，所以用dont care？存疑
	//				rsrc_usage_tar.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				rsrc_usage_tar.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}

	////PASSES EXECUTION(TODO:ORGANIZE PASSES INTO GROUPS FOR PARALLEL EXECUTION)

	//for (const auto pass_node_ptr : final_execution_order)
	//{
	//	ExecutePass(cmb, pass_node_ptr);
	//}

	//std::cout << "Render graph command recording finished" << std::endl;
	return true;
}

void RenderGraphV0::DependencyGraph::GeneratePassNodeDepen()
{
	/// DEPENDENCY GENERATION

	for (const auto &gfx_node : gfx_pass_nodes)
	{
		//对于每一个inlet，会对其中的资源进行读或者写
		//case 0:如果是对这个资源进行了读取，则要去链上寻找前一个写入该资源的pass
		//case 1:如果是对这个资源进行了写入，则要去链上寻找前一个读取该资源的pass（因为会写入会暴力覆盖之前pass中写入的内容）

		//TODO: 测试subresource ranges overlap
		//TODO: 多线程优化

		for (auto &[_, inlet] : gfx_node->ins_buf)
		{
			//在当前pass中的使用方式usage
			const auto usage     = inlet.GetRsrcUsage();
			auto &     buf_usage = *usage;

			//如果使用了读的方式，则
			if (buf_usage.GetAccessType() == Vk::AccessType::Read)
			{
				PassNode *                                                                            previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                  providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					//providing_outlet_itor一定是outlet
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
					{
						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
						//{
						//	gfx_node->passes_depen_set.insert(previous_pass);
						//}

						break;
					}
				}
			}
			else if (buf_usage.GetAccessType() == Vk::AccessType::Write)
			{
				PassNode *                                                                            previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                  providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();

						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
					{
						if (inlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
						{
							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
							if (previous_pass != gfx_node.get())
							{
								gfx_node->passes_depen_set.insert(previous_pass);
							}
						}
						break;
					}
				}
			}
		}

		for (auto &[_, outlet] : gfx_node->outs_buf)
		{
			//在当前pass中的使用方式usage
			const auto usage     = outlet.GetRsrcUsage();
			auto &     buf_usage = *usage;

			//如果使用了读的方式，则
			if (buf_usage.GetAccessType() == Vk::AccessType::Read)
			{
				PassNode *                                                                            previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                  providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					//providing_outlet_itor一定是outlet
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
					{
						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
						//{
						//	gfx_node->passes_depen_set.insert(previous_pass);
						//}

						break;
					}
				}
			}
			else if (buf_usage.GetAccessType() == Vk::AccessType::Write)
			{
				PassNode *                                                                            previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                  providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();

						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
					{
						if (outlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
						{
							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
							if (previous_pass != gfx_node.get())
							{
								gfx_node->passes_depen_set.insert(previous_pass);
							}
						}
						break;
					}
				}
			}
		}

		for (auto &[_, inlet] : gfx_node->ins_uni_tex)
		{
			const auto usage     = inlet.GetRsrcUsage();
			auto &     tex_usage = *usage;

			if (tex_usage.GetAccessType() == Vk::AccessType::Read)
			{
				PassNode *                                                                                  previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                        providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					//providing_outlet_itor一定是outlet
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
					{
						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
						//{
						//	gfx_node->passes_depen_set.insert(previous_pass);
						//}

						break;
					}
				}
			}

			else if (tex_usage.GetAccessType() == Vk::AccessType::Write)
			{
				PassNode *                                                                                  previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                        providing_pass_and_source_outlet_itr = inlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();

						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
					{
						if (inlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
						{
							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
							if (previous_pass != gfx_node.get())
							{
								gfx_node->passes_depen_set.insert(previous_pass);
							}
						}
						break;
					}
				}
			}
		}

		for (auto &[_, outlet] : gfx_node->outs_uni_tex)
		{
			const auto usage     = outlet.GetRsrcUsage();
			auto &     tex_usage = *usage;

			if (tex_usage.GetAccessType() == Vk::AccessType::Read)
			{
				PassNode *                                                                                  previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                        providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					//providing_outlet_itor一定是outlet
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的（如何处理来自上一帧甚至上上一帧的资源？比如TAA的情况） 或者 资源是在当前pass创建的（）。
					{
						//established资源在对应的创建pass中一定会被写入而不是被读取，所以在上一个loop就已经加入进了passes_depen_set
						//if (inlet.second.GetItrInRsrcMap()->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
						//{
						//	gfx_node->passes_depen_set.insert(previous_pass);
						//}

						break;
					}
				}
			}

			else if (tex_usage.GetAccessType() == Vk::AccessType::Write)
			{
				PassNode *                                                                                  previous_pass = gfx_node.get();
				std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator previous_out_itr;
				auto                                                                                        providing_pass_and_source_outlet_itr = outlet.GetProvidingOutletItrAndPass();

				while (true)
				{
					if (providing_pass_and_source_outlet_itr)
					{
						auto providing_itr  = providing_pass_and_source_outlet_itr->second;
						auto providing_pass = providing_pass_and_source_outlet_itr->first;

						previous_pass    = providing_pass;
						previous_out_itr = providing_itr;

						const auto providing_pass_access_type = providing_itr->second.GetRsrcUsage()->GetAccessType();

						if (providing_pass_access_type == Vk::AccessType::Read)
						{
							gfx_node->passes_depen_set.insert(providing_pass);
							break;
						}
						if (providing_pass_access_type == Vk::AccessType::Write)
						{
							providing_pass_and_source_outlet_itr = providing_itr->second.GetProvidingOutletItrAndPass();
						}
					}
					else        //如果出现提供资源的pass不存在的情况，那么说明资源一开始是来自rendergraph之外的 或者 资源是在当前pass创建的。
					{
						if (outlet.GetUnderlyingRsrcItr()->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
						{
							//在资源非导入的情况下，如果所有pass链上的pass都是写入，则至少需要等第一个pass创建好资源！
							if (previous_pass != gfx_node.get())
							{
								gfx_node->passes_depen_set.insert(previous_pass);
							}
						}
						break;
					}
				}
			}
		}

		std::remove(gfx_node->passes_depen_set.begin(), gfx_node->passes_depen_set.end(), nullptr);
	}

	for (const auto &gfx_node : gfx_pass_nodes)
	{
		for (const auto &ptr_before_pass : gfx_node->passes_depen_set)
		{
			ptr_before_pass->passes_depen_on_cur_pass_set.insert(gfx_node.get());
		}
	}
}

void RenderGraphV0::DependencyGraph::VisitNode(size_t cur_index, PassNode *cur_node_pass, std::vector<bool> &visited, std::vector<bool> &on_stack, std::vector<PassNode *> &topologically_sorted_nodes)
{
	if (visited[cur_index] && on_stack[cur_index])
	{
		throw std::runtime_error("found a circular dependency and must abort.");
	}
	if (visited[cur_index])
	{
		return;
	}

	visited[cur_index]  = true;
	on_stack[cur_index] = true;

	for (PassNode *adj_list : cur_node_pass->passes_depen_on_cur_pass_set)
	{
		VisitNode(FindIndexInPassNodeArray(adj_list, gfx_pass_nodes), adj_list, visited, on_stack, topologically_sorted_nodes);
	}

	topologically_sorted_nodes.push_back(cur_node_pass);
	on_stack[cur_index] = false;
}

size_t RenderGraphV0::DependencyGraph::FindIndexInPassNodeArray(PassNode *node_pass, const std::vector<std::unique_ptr<GraphicsPassNode>> &all_nodes)
{
	for (auto gfx_node_itr = all_nodes.begin(); gfx_node_itr < all_nodes.end(); ++gfx_node_itr)
	{
		if (gfx_node_itr->get() == node_pass)
		{
			return std::distance(all_nodes.begin(), gfx_node_itr);
		}
	}
	throw std::runtime_error("pass not found!");
}

RenderGraphV0::PassNode *RenderGraphV0::DependencyGraph::FindLastAccessingPass(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const
{
	std::ranges::sort(passes_access_this_rsrc.begin(), passes_access_this_rsrc.end(),
	                  [&final_exe_order](const PassNode *fir, const PassNode *sec) -> bool {
		                  const auto fir_itr = std::find(final_exe_order.begin(), final_exe_order.end(), fir);
		                  const auto sec_itr = std::find(final_exe_order.begin(), final_exe_order.end(), sec);
		                  return fir_itr < sec_itr;
	                  }

	);

	return *passes_access_this_rsrc.rbegin();
}

void RenderGraphV0::DependencyGraph::SortAccessingPasses(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const
{
	std::sort(passes_access_this_rsrc.begin(), passes_access_this_rsrc.end(),
	          [&final_exe_order](const PassNode *fir, const PassNode *sec) -> bool {
		          const auto fir_itr = std::find(final_exe_order.begin(), final_exe_order.end(), fir);
		          const auto sec_itr = std::find(final_exe_order.begin(), final_exe_order.end(), sec);
		          return fir_itr < sec_itr;
	          }

	);
}

VkRenderpassBaseRG::Type RenderGraphV0::GraphicsPassNode::GetRenderpassType()
{
	return VkRenderpassBaseRG::Type::Graphics;
}

void RenderGraphV0::GraphicsPassNode::PreExecute()
{
	renderpass->Init();
}

void RenderGraphV0::GraphicsPassNode::Execute(VkCommandBuffer cmd_buf)
{
	renderpass->Execute(cmd_buf);
}

//
////接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	//const auto rsrc_name_outside_RG = rsrc_name + std::string("_") + std::string("Imported");        //外部引入资源，无pass后缀
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //外部引入资源
//
//	//首先看是否已经存在于buffers map中，如果没有就首次创建
//	auto rsrc_itr = rg.buffers_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.buffers_map.end())
//	{
//		const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_buf);        //键值和emplace构造元素所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//	//创建当前pass的资源的导入口
//	const auto cur_in_itr = ins_buf.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *buf_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把usage和资源本身联系起来。
//	buf_usage->buffer = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//资源使用方式存储
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
////接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出*************************************
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//	//从其他pass的outlets中寻找对应资源
//	const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//	if (source_pass.outs_buf.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//给source outlet增加使用它的pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//给ItrInRsrcMap 增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//把资源使用方式和虚拟资源联系起来
//	buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//创建当前pass的资源的导入口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *buf_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//给inlet赋值提供它的pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//资源使用方式存储
//	buf_usages.push_back(std::move(buf_usage));
//	//[此资源不需要在当前pass实体化]
//
//	return *this;
//}
//
////buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//当前资源含有descriptor，资源就是在当前pass创建。
//
//	//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
//	//const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//
//	//资源的命名方式为：资源名 + _
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//用传入的lambda函数改变descriptor
//	descriptor_modifier(buf_descriptor);
//
//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//	const auto itr = rg.buffers_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, buf_descriptor);
//	//确保用户没有重复添加
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	buf_usage->buffer = itr.first;
//
//	//给ItrInRsrcMap增加使用它的pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *buf_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//资源使用方式存储
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源需要在当前pass实体化]
//	buf_init_list.push_back(itr.first);
//
//	return *this;
//}
//
////buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//const auto rsrc_name_imported_outside = rsrc_name + std::string("_") + std::string("Imported");
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//	//首先看imported virtual resource是否已经存在了
//	auto rsrc_itr = rg.buffers_map.find(rsrc_name_imported_outside);
//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//	//如果virtual resource不存在，则创建:
//	if (rsrc_itr == rg.buffers_map.end())
//	{
//		const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_buf);        //键值和emplace构造所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	buf_usage->buffer = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *buf_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
////接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//先找一找  源头pass的导出接口是否确实提供了需要的资源
//
//	const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_buf.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错，抛出异常
//	}
//
//	//source pass中的outlet添加accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//把资源使用方式和虚拟资源本身联系起来
//	buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *buf_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
////************************************************************************************************
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
//	//首先看是否已经存在于buffers map中，如果没有就重新创建
//	auto rsrc_itr = rg.textures_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.textures_map.end())
//	{
//		const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//	//创建当前pass的资源的导入口
//	const auto cur_in_itr = ins_tex.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *tex_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把usage和资源本身联系起来。
//	tex_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//资源使用方式存储
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//	//从其他pass的outlets中寻找对应资源
//	const auto source_outlet_itr = source_pass.outs_tex.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//	if (source_pass.outs_tex.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//给source outlet增加使用它的pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//把资源使用方式和虚拟资源联系起来
//	tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导入口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_tex.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *tex_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//给inlet赋值提供它的pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//资源使用方式存储
//	tex_usages.push_back(std::move(tex_usage));
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	//当前函数传入参数含有descriptor，资源就是在当前pass创建。
//
//	//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//用传入的lambda函数改变descriptor
//	descriptor_modifier(tex_descriptor);
//
//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//	const auto itr = rg.textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//	//确保用户没有重复添加
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	tex_usage->texture = itr.first;
//
//	//给ItrInRsrcMap增加使用它的pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *tex_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//资源使用方式存储
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源需要在当前pass实体化]
//	tex_init_list.push_back(itr.first);
//
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//	//首先看imported virtual resource是否已经存在了
//	auto rsrc_itr = rg.textures_map.find(rsrc_name_imported_outside);
//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//	//如果virtual resource不存在，则创建:
//	if (rsrc_itr == rg.textures_map.end())
//	{
//		const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	tex_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *tex_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//先找一找源头pass的导出接口是否确实提供了需要的资源
//	const auto source_outlet_itr = source_pass.outs_tex.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_tex.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错
//	}
//
//	//source pass中的outlet添加accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//把资源使用方式和虚拟资源本身联系起来
//	tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *tex_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
////************************************************************************************************
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
//	//首先看是否已经存在于buffers map中，如果没有就重新创建
//	auto rsrc_itr = rg.attachments_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.attachments_map.end())
//	{
//		const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//	//创建当前pass的资源的导入口
//	const auto cur_in_itr = ins_attach.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *attach_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把usage和资源本身联系起来。
//	attach_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//资源使用方式存储
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//	//从其他pass的outlets中寻找对应资源
//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//	if (source_pass.outs_attach.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//给source outlet增加使用它的pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//把资源使用方式和虚拟资源联系起来
//	attach_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导入口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *attach_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//给inlet赋值提供它的pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//资源使用方式存储
//	attach_usages.push_back(std::move(attach_usage));
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	//当前资源含有descriptor，资源就是在当前pass创建。
//
//	//资源的命名方式为：资源名 + _
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//用传入的lambda函数改变descriptor
//	descriptor_modifier(tex_descriptor);
//
//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//	const auto itr = rg.attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//	//确保用户没有重复添加
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	attach_usage->texture = itr.first;
//	//给ItrInRsrcMap增加使用它的pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *attach_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//资源使用方式存储
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源需要在当前pass实体化]
//	attch_init_list.push_back(itr.first);
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//	//首先看imported virtual resource是否已经存在了
//	auto rsrc_itr = rg.attachments_map.find(rsrc_name_imported_outside);
//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//	//如果virtual resource不存在，则创建:
//	if (rsrc_itr == rg.attachments_map.end())
//	{
//		const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	attach_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *attach_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//先找一找源头pass的导出接口是否确实提供了需要的资源
//
//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_attach.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错
//	}
//
//	//source pass中的outlet添加accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//把资源使用方式和虚拟资源本身联系起来
//	attach_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *attach_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//

//这里面所有容器都不能删除元素，否则通过index进行的映射就会出问题，想办法封装一层，不要提供删除的接口
////**************************************************************
//std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, ResourceSlot>> syn_infos;
////**************************************************************
//std::vector<ResourceSlot> rsrc_init_list;
//std::vector<ResourceSlot> rsrc_recycle_list;
////**************************************************************
//std::vector<ResourceSlot> rsrc_state_changing_list;
////**************************************************************
//std::unordered_map<ResourceSlot, VirtualResource::RsrcAccessType> rsrc_accessing_types;
////**************************************************************
//class PassNode;

//class TexEdge
//{
//  public:
//	TexEdge(const PassNode &f, const PassNode &t, const VkTexUsageInfoRG &usage_) :
//	    from(f), to(t), usage(usage_)
//	{
//	}

//	~TexEdge() = default;

//	TexEdge(const TexEdge &) = default;
//	TexEdge(TexEdge &&)      = default;

//	TexEdge &operator=(TexEdge &&) = delete;
//	TexEdge &operator=(const TexEdge &) = delete;

//  public:
//	const PassNode &        from;
//	const PassNode &        to;
//	const VkTexUsageInfoRG &usage;
//};

//class BufEdge
//{
//  public:
//	BufEdge(const PassNode &f, const PassNode &t, const VkBufUsageInfoRG &usage_) :
//	    from(f), to(t), usage(usage_)
//	{
//	}

//	~BufEdge() = default;

//	BufEdge(const BufEdge &) = default;
//	BufEdge(BufEdge &&)      = default;

//	BufEdge &operator=(BufEdge &&) = delete;
//	BufEdge &operator=(const BufEdge &) = delete;

//  public:
//	const PassNode &        from;
//	const PassNode &        to;
//	const VkBufUsageInfoRG &usage;
//};

//class AttachEdge
//{
//  public:
//	AttachEdge(const PassNode &f, const PassNode &t, const VkAttachmentInfo::WithinPassRG &usage_) :
//	    from(f), to(t), usage(usage_)
//	{
//	}

//	~AttachEdge() = default;

//	AttachEdge(const AttachEdge &) = default;
//	AttachEdge(AttachEdge &&)      = default;

//	AttachEdge &operator=(AttachEdge &&) = delete;
//	AttachEdge &operator=(const AttachEdge &) = delete;

//  public:
//	const PassNode &                      from;
//	const PassNode &                      to;
//	const VkAttachmentInfo::WithinPassRG &usage;
//};
