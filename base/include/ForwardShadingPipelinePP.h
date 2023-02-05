#pragma once

//#include "VkPipelinePP.h"
//
//class ForwardShadingPipelinePP : public VkPipelinePP
//{
//public:
//	ForwardShadingPipelinePP(VkGraphicsComponent &gfx_);
//
//	[[nodiscard]] std::shared_ptr<VkPipelinePP> Clone() const override final;
//
//protected:
//	void InitInputAssemblyStateCI() override final;
//
//	void InitRasterizationStateCI() override final;
//
//	void InitDepthStencilStateCI() override final;
//
//	void InitMultisampleStateCI() override final;
//
//	void InitDynamicState() override final;
//
//	void InitColorBlendStateCI() override final;
//
//	void InitViewPortStateCI() override final;
//
//	void InitVertexInputStateCI() override final;
//
//	void InitRenderingCI() override final;
//
//
//
//
//};




#include "VkPipelinePPFactory.h"

class ForawdShadingPipelinePPFactory : public VkPipelinePPFactory
{
  public:
	ForawdShadingPipelinePPFactory(VkGraphicsComponent &gfx_);
	~ForawdShadingPipelinePPFactory() override = default;

	ForawdShadingPipelinePPFactory(const ForawdShadingPipelinePPFactory &) = delete;
	ForawdShadingPipelinePPFactory(ForawdShadingPipelinePPFactory &&)      = delete;

	ForawdShadingPipelinePPFactory &operator=(const ForawdShadingPipelinePPFactory &) = delete;
	ForawdShadingPipelinePPFactory &operator=(ForawdShadingPipelinePPFactory &&) = delete;

  protected:
	void ConstructInputAssemblyStateCI(VkPipelinePP &pp) final override;
	void ConstructRasterizationStateCI(VkPipelinePP &pp) final override;
	void ConstructDepthStencilStateCI(VkPipelinePP &pp) final override;
	void ConstructMultisampleStateCI(VkPipelinePP &pp) final override;
	void ConstructDynamicState(VkPipelinePP &pp) final override;
	void ConstructColorBlendStateCI(VkPipelinePP &pp) final override;
	void ConstructVertexInputStateCI(VkPipelinePP &pp) final override;
	void ConstructViewPortStateCI(VkPipelinePP &pp) final override;
	void ConstructRenderingCI(VkPipelinePP &pp) final override;
};

