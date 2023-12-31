#include "RenderGraphPass.h"

RenderGraphV0::PassNode::PassNode(const std::string name_, VkGraphicsComponent &gfx_) :
    name(name_), root_pass_tag(nullptr), is_rootpass(false), gfx(gfx_)
{
}

RenderGraphV0::PassNode::~PassNode()
{
}

void RenderGraphV0::PassNode::SetAsRootPass()
{
	is_rootpass = true;
}

void RenderGraphV0::PassNode::SetAsNonRootPass()
{
	is_rootpass = false;
}

bool RenderGraphV0::PassNode::IsRootPass()
{
	return is_rootpass;
}

void RenderGraphV0::PassNode::TagCurRootPass(PassNode *cur_pass)
{
	root_pass_tag = cur_pass;
}

void RenderGraphV0::PassNode::AddSubGraphPass(PassNode *ps)
{
	if (!is_rootpass)
	{
		throw std::runtime_error("This is not a root pass");
	}

	subgraph_pass.insert(ps);
}

VkRenderpassBaseRG::Type RenderGraphV0::PassNode::GetAccumulatedType()
{
	return subgraph_accumulated_type;
}

void RenderGraphV0::PassNode::AccumulateQueueRequirement()
{
	if (!is_rootpass)
	{
		subgraph_accumulated_type = VkRenderpassBaseRG::Type::None;
	}
	else
	{
		subgraph_accumulated_type = GetRenderpassType();
		for (auto pass : subgraph_pass)
		{
			subgraph_accumulated_type = static_cast<VkRenderpassBaseRG::Type>((subgraph_accumulated_type | pass->GetRenderpassType()));
		}
	}
}

VkRenderpassBaseRG::Type RenderGraphV0::PassNode::GetRenderpassType()
{
	return VkRenderpassBaseRG::Type::None;
}
