#pragma once

//#include "VkPipelinePP.h"
//
//class ImguiPipePP : public VkPipelinePP
//
//{
//public:
//	ImguiPipePP(VkGraphicsComponent &gfx_);
//
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
//	[[nodiscard]]std::shared_ptr<VkPipelinePP> Clone() const override final;
//
//};



#include "VkPipelinePPFactory.h"

class ImguiPipePPFactory : public VkPipelinePPFactory
{
public:
	ImguiPipePPFactory(VkGraphicsComponent &gfx_);
	~ImguiPipePPFactory() override = default;

	ImguiPipePPFactory(const ImguiPipePPFactory &) = delete;
	ImguiPipePPFactory(ImguiPipePPFactory &&)      = delete;

	ImguiPipePPFactory &operator=(const ImguiPipePPFactory &) = delete;
	ImguiPipePPFactory &operator=(ImguiPipePPFactory &&) = delete;

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
