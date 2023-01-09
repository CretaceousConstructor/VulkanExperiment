#pragma once
#include "ModelStrcture.h"
#include "VkDescriptorManager.h"
#include "VkGraphicsComponent.h"
#include "VkManagerBundle.h"
#include "VkMetaInfo.h"

class VkMaterial
{
  public:
	VkMaterial(VkGraphicsComponent &gfx_);
	virtual ~VkMaterial() = default;

	VkMaterial()                   = delete;
	VkMaterial(const VkMaterial &) = delete;
	VkMaterial &operator=(const VkMaterial &) = delete;
	VkMaterial(VkMaterial &&)                 = delete;
	VkMaterial &operator=(VkMaterial &&) = delete;

	virtual void                         AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images) = 0;
	[[nodiscard]] const VkDescriptorSet &GetDescriptorSet() const;

	void                            SetPipeline(VkPipeline pipeline_);
	[[nodiscard]] const VkPipeline &GetPipeline() const;

	virtual void                   ModifyPipelineCI(VkPipelinePP &pipeline_CI) = 0;
	[[nodiscard]] virtual uint32_t GetRequiredDescirpotrCount() const          = 0;

  protected:
	VkDescriptorSet descriptor_set{};
	VkPipeline      pipeline{};

  protected:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};
