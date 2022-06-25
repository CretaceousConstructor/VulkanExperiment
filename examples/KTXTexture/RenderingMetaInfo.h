#pragma once
#include "VkBufferBundle.h"
#include "VkImageBundle.h"
#include "VkMetaInfo.h"
#include "VkModel.h"


namespace KtxRenderer
{
	//*****************************************RENDERING LAYOUT******************************************
	///GLOBAL RESOURCES
	inline constexpr DescriptorPoolMetaInfo      pool_main_thread{.thread_id = 0};

	///RENDERPASS 0
	inline constexpr size_t                    renderpass0 = 0;
	inline constexpr DescriptorSetLayoutMetaInfo des_set_layout_0{.id = 0};
	inline constexpr DescriptorSetMetaInfo       des_set0{.pool = pool_main_thread, .layout = des_set_layout_0,.id = 0
	};

	const PipelineLayoutMetaInfo          layout_0_0{
        .descriptor_layout_ids_vec{des_set_layout_0},
        .id = 0
	};
	const PipelineMetaInfo pipe_0_0{.pass = renderpass0, .subpass = 0, .pipelayout_id = layout_0_0};

	//UBO DATA
	struct UboData
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec4 eyepos;
		alignas(4) float lodBias = 0.0f;
	};

	//vertex layout
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


	//COMMON RESOURCES
	class RenderpassCommonResources
	{
	  public:
		//UNIFORM BUFFER
		UboData                         ubo{};
		std::shared_ptr<VkBufferBundle> uniform_buffers;
		//TEXTURE
		std::shared_ptr<VkTexture> ktx_texure;

		//ATTACHMENT
		std::shared_ptr<VkImageBundle> swapchain_images;
		std::shared_ptr<VkImageBundle> depth_attachments;
		//MODELS
		std::shared_ptr<VkModel<Vertex>> quad_model;

	};

}
