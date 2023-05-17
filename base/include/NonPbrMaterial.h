#pragma once

#include "Vk.h"
#include "VkMaterial.h"

class NonPbrMaterial : public VkMaterial
{
  public:
	NonPbrMaterial(VkGraphicsComponent &gfx_);
	~NonPbrMaterial() override = default;

	NonPbrMaterial()                       = delete;
	NonPbrMaterial(const NonPbrMaterial &) = delete;
	NonPbrMaterial &operator=(const NonPbrMaterial &) = delete;
	NonPbrMaterial(NonPbrMaterial &&)                 = delete;
	NonPbrMaterial &operator=(NonPbrMaterial &&) = delete;

	glm::vec4 baseColorFactor{glm::vec4(1.0f)};
	uint32_t  baseColorTextureIndex{};

	//float    metallicFactor{1.0f};
	//float    roughnessFactor{1.0f};
	//uint32_t metallicRoughnessTextureIndex{};

	//uint32_t metallicTextureIndex{};
	//uint32_t roughnessTextureIndex{};

	uint32_t normalTextureIndex{};

	uint32_t emissiveTextureIndex{};

	uint32_t occlusionTextureIndex{};

  public:
	enum class AlphaMode
	{
		ALPHAMODE_OPAQUE,
		ALPHAMODE_MASK,
		ALPHAMODE_BLEND
	};

	AlphaMode alphaMode{AlphaMode::ALPHAMODE_OPAQUE};
	float     alphaCutoff{0.};
	bool      doubleSided{false};

	//***********************************************************************
	[[nodiscard]] constexpr uint32_t GetRequiredDescirpotrCount() const override;

	VkDescriptorSet AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images) override;
	void ModifyPipelineCI(VkPipelinePP &pipeline_CI) override;

	static VkDescriptorSetLayout GetDescriptorSetLayout();

  public:
	static VkDescriptorSetLayout CreateDesciptorSetLayout(const VkDeviceManager &device_manager);
	static VkPipelineLayout      CreatePipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges);


	static void Register(VkGraphicsComponent &gfx);
	static void UnRegister(VkGraphicsComponent &gfx);



  private:
	inline static VkDescriptorSetLayout desc_layout{nullptr};
	//inline static VkPipelineLayout      pipe_layout{nullptr};

  private:
	struct MaterialSpecializationData
	{
		bool  alphaMask;
		float alphaMaskCutoff;
	};

	MaterialSpecializationData            material_specialization_data{};
	std::vector<VkSpecializationMapEntry> specialization_map_entries;
};
