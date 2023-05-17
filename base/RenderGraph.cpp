#include "RenderGraph.h"

void RenderGraph::Register(const VkRenderpassBase &pass)
{
	std::string pass_name{typeid(pass).name()};
}

void RenderGraph::Register(const VkBufferBase &buffer, VkRenderpassBase::Type pass_type, std::string name)
{
}
