#pragma once

#include "VkPipelinePP.h"




class ForwardShadingPipelinePP : public VkPipelinePP
{


public:
	ForwardShadingPipelinePP(VkGraphicsComponent &gfx_);

	void InitInputAssemblyStateCI() override final;

	void InitRasterizationStateCI() override final;

	void InitDepthStencilStateCI() override final;

	void InitMultisampleStateCI() override final;

	void InitDynamicState() override final;

	void InitColorBlendStateCI() override final;

	void InitViewPortStateCI() override final;

	void InitVertexInputStateCI() override final;

	void InitRenderingCI() override final;

	[[nodiscard]] std::shared_ptr<VkPipelinePP> Clone() const override final;










};
