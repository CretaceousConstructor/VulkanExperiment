#pragma once
#include "VkBufferBase.h"
#include "VkRenderpassBase.h"
#include <typeindex>

class RenderGraph
{
  public:
	//struct PassParameter
	//{
	//	std::vector<ResourceRef> ShaderResources;
	//	std::vector<ResourceRef> RenderTargets;
	//};
	enum class ResourcesUsageFlag
	{
		Read,
		Write,
		ComputeRW,
		RenderTarget,
		DepthStencil,
	};

	//Register passes or resources.
	void Register(const VkRenderpassBase &pass);
	void Register(const VkBufferBase &buffer, VkRenderpassBase::Type pass_type, std::string name);
};
