#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkPipelineWrapper.h"
#include "VkRenderpassWrapper.h"
#include "VkSwapChainManager.h"
#include <memory>

class VkPipelineBuilder
{
  public:
	VkPipelineBuilder(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager);
	~VkPipelineBuilder() = default;

	VkPipelineBuilder()                          = delete;
	VkPipelineBuilder(const VkPipelineBuilder &) = delete;
	VkPipelineBuilder &operator=(const VkPipelineBuilder &) = delete;
	VkPipelineBuilder(VkPipelineBuilder &&)                 = delete;
	VkPipelineBuilder &operator=(VkPipelineBuilder &&) = delete;

  public:
	std::shared_ptr<VkPipelineWrapper> GetPipline(PipelineMetaInfo pipeline_meta_info, VkRenderpassWrapper &renderpass, const std::vector<ShaderWrapper::ShaderInfo> shader_infos);

  public:
	struct Vertex
	{
		glm::vec3                                             pos;
		glm::vec3                                             normal;
		glm::vec2                                             uv;
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.resize(3);

			attributeDescriptions[0].binding  = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset   = offsetof(Vertex, pos);

			attributeDescriptions[1].binding  = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset   = offsetof(Vertex, normal);

			attributeDescriptions[2].binding  = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset   = offsetof(Vertex, uv);

			return attributeDescriptions;
		}
	};








  private:
	VkDeviceManager &   device_manager;
	VkSwapChainManager &swapchain_manager;

  private:
	void ResetResultPtr();
	void RestoreToDefaultState();

	void BuildShader(const std::vector<ShaderWrapper::ShaderInfo> shader_infos) const;
	void BuildPipeline(PipelineMetaInfo pipeline_meta_info, VkRenderpassWrapper &renderpass);

  private:
	std::vector<VkVertexInputBindingDescription>              VIBDS;
	std::pair<std::vector<VkViewport>, std::vector<VkRect2D>> view_port_scissor_pair;
	std::vector<VkPipelineColorBlendAttachmentState>          color_blend_attachments;
	std::vector<VkDynamicState>                               dynamic_states;

	VkPipelineDynamicStateCreateInfo       dynamic_state_CI{};
	VkPipelineVertexInputStateCreateInfo   vertex_input_state_CI{};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
	VkPipelineViewportStateCreateInfo      viewport_state_CI{};
	VkPipelineRasterizationStateCreateInfo rasterization_state_CI{};
	VkPipelineMultisampleStateCreateInfo   multisample_state_CI{};
	VkPipelineDepthStencilStateCreateInfo  depth_stencil_CI{};
	VkPipelineColorBlendStateCreateInfo    color_blend_state_CI{};

	VkGraphicsPipelineCreateInfo pipeline_create_CI{};

  private:
	bool state_has_changed = false;

	std::shared_ptr<VkPipelineWrapper> pipeline;
};