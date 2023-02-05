#pragma once

#include "VkGraphicsComponent.h"
#include "VkPipelinePP.h"

class VkPipelinePPFactory
{
  public:
	VkPipelinePPFactory(VkGraphicsComponent &gfx_);
	virtual ~VkPipelinePPFactory() = default;

	VkPipelinePPFactory(const VkPipelinePPFactory &) = delete;
	VkPipelinePPFactory(VkPipelinePPFactory &&)      = delete;

	VkPipelinePPFactory &operator=(const VkPipelinePPFactory &) = delete;
	VkPipelinePPFactory &operator=(VkPipelinePPFactory &&) = delete;

	std::unique_ptr<VkPipelinePP> GetPipelinePP();
	void         RestoreToDefaultState(VkPipelinePP &pp);


  protected:
	virtual void ConstructInputAssemblyStateCI(VkPipelinePP &pp) = 0;
	virtual void ConstructRasterizationStateCI(VkPipelinePP &pp) = 0;
	virtual void ConstructDepthStencilStateCI(VkPipelinePP &pp)  = 0;
	virtual void ConstructMultisampleStateCI(VkPipelinePP &pp)   = 0;
	virtual void ConstructDynamicState(VkPipelinePP &pp)         = 0;
	virtual void ConstructColorBlendStateCI(VkPipelinePP &pp)    = 0;
	virtual void ConstructVertexInputStateCI(VkPipelinePP &pp)   = 0;
	virtual void ConstructViewPortStateCI(VkPipelinePP &pp)      = 0;
	virtual void ConstructRenderingCI(VkPipelinePP &pp)          = 0;

  protected:
	VkGraphicsComponent &     gfx;
	const VkSwapchainManager &swapchain_manager;
};

//class ForawdShadingPipelinePPFactory : public VkPipelinePPFactory
//{
//  public:
//	ForawdShadingPipelinePPFactory(VkGraphicsComponent &gfx_);
//	~ForawdShadingPipelinePPFactory() override = default;
//
//	ForawdShadingPipelinePPFactory(const ForawdShadingPipelinePPFactory &) = delete;
//	ForawdShadingPipelinePPFactory(ForawdShadingPipelinePPFactory &&)      = delete;
//
//	ForawdShadingPipelinePPFactory &operator=(const ForawdShadingPipelinePPFactory &) = delete;
//	ForawdShadingPipelinePPFactory &operator=(ForawdShadingPipelinePPFactory &&) = delete;
//
//  protected:
//	void ConstructInputAssemblyStateCI(VkPipelinePP &pp) final override;
//	void ConstructRasterizationStateCI(VkPipelinePP &pp) final override;
//	void ConstructDepthStencilStateCI(VkPipelinePP &pp) final override;
//	void ConstructMultisampleStateCI(VkPipelinePP &pp) final override;
//	void ConstructDynamicState(VkPipelinePP &pp) final override;
//	void ConstructColorBlendStateCI(VkPipelinePP &pp) final override;
//	void ConstructVertexInputStateCI(VkPipelinePP &pp) final override;
//	void ConstructViewPortStateCI(VkPipelinePP &pp) final override;
//	void ConstructRenderingCI(VkPipelinePP &pp) final override;
//};
