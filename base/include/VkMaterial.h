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

	virtual ~VkMaterial()          = default;
	VkMaterial()                   = delete;
	VkMaterial(const VkMaterial &) = delete;
	VkMaterial &operator=(const VkMaterial &) = delete;
	VkMaterial(VkMaterial &&)                 = delete;
	VkMaterial &operator=(VkMaterial &&) = delete;

	virtual void AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images) = 0;
	//virtual VkDescriptorSetLayout CreateDesciptorSetLayout()                                                                                                                                                                                 = 0;
	//static VkPipelineLayout      GetPipelineLayout(const VkDeviceManager& device_manager,const std::vector<VkDescriptorSetLayout> &other_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges);
	//static VkDescriptorSetLayout CreateDesciptorSetLayout(const VkDeviceManager& device_manager);

	void                                 SetPipeline(VkPipeline pipeline_);
	[[nodiscard]] const VkPipeline &     GetPipeline() const;
	[[nodiscard]] const VkDescriptorSet &GetDescriptorSet() const;

	virtual void     ModifyPipelineCI(VkPipelinePP &pipeline_CI) = 0;
	virtual uint32_t GetRequiredDescirpotrCount()                           = 0;

  protected:
	VkDescriptorSet descriptor_set{};
	VkPipeline      pipeline{};

  protected:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};

