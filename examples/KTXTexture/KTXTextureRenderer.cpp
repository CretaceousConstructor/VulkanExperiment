#include "KTXTextureRenderer.h"

void KTXTextureRenderer::CreateTextureImages()
{
	VkFormat format_of_texture = VK_FORMAT_R8G8B8A8_SRGB;

	ktx_texure.InitKTXTexture(std::string("../../data/textures/metalplate01_rgba.ktx"), device_manager, window, transfer_command_pool, format_of_texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	ktx_texure.InitTextureView(format_of_texture, VK_IMAGE_ASPECT_COLOR_BIT);
	ktx_texure.InitSampler();
}

void KTXTextureRenderer::PrepareModels()
{
	std::vector<Vertex> vertices =
	    {
	        {glm::vec3{1.0f, 1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 0.f}},
	        {glm::vec3{-1.0f, 1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}},
	        {glm::vec3{-1.0f, -1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 1.0f}},
	        {glm::vec3{1.0f, -1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 1.0f}}};
	// Setup indices
	std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

	quad_model = std::make_unique<VkModel<Vertex>>(vertices, indices, device_manager, window->GetSurface(), transfer_command_buffer);
}

void KTXTextureRenderer::CreateDescriptorPool()
{
	{
		// Create the global descriptor pool
		std::array<VkDescriptorPoolSize, 2> poolSizes{};

		poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 3;        //3        uniform_buffers  ��3֡��1*3 = 3

		poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCI = {};
		descriptorPoolCI.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCI.poolSizeCount              = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCI.pPoolSizes                 = poolSizes.data();

		// Max. number of descriptor sets that can be allocated from this pool
		descriptorPoolCI.maxSets = static_cast<uint32_t>(swapchain_manager->GetSwapImageCount());        //һ֡һ��set�Ƿ��˷ѣ����ֻ��һ��set�᲻����ͬ������?
		if (vkCreateDescriptorPool(device_manager->GetLogicalDeviceRef(), &descriptorPoolCI, nullptr, &descriptor_pool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
}
void KTXTextureRenderer::CreateDescriptorSetLayout()
{
	{
		std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass0;
		VkDescriptorSetLayoutBinding              LayoutBindingSubpass0_temp{};

		// Descriptor set layout binding for passing matrices;
		// Binding 0 : Vertex shader uniform buffer for passing matrices
		LayoutBindingSubpass0_temp.binding            = 0;
		LayoutBindingSubpass0_temp.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LayoutBindingSubpass0_temp.descriptorCount    = 1;
		LayoutBindingSubpass0_temp.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
		LayoutBindingSubpass0_temp.pImmutableSamplers = nullptr;        // Optional
		LayoutBindingSubpass0.push_back(LayoutBindingSubpass0_temp);

		// Binding 1 : Fragment shader image sampler
		LayoutBindingSubpass0_temp.binding            = 1;
		LayoutBindingSubpass0_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		LayoutBindingSubpass0_temp.descriptorCount    = 1;
		LayoutBindingSubpass0_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		LayoutBindingSubpass0_temp.pImmutableSamplers = nullptr;        // Optional
		LayoutBindingSubpass0.push_back(LayoutBindingSubpass0_temp);

		VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass0{};
		LayoutBindingCISubpass0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutBindingCISubpass0.bindingCount = (uint32_t) LayoutBindingSubpass0.size();        //the amount of VkDescriptorSetLayoutBinding
		LayoutBindingCISubpass0.pBindings    = LayoutBindingSubpass0.data();
		if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass0, nullptr, &descriptor_set_layout_write_subpass0) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
}

void KTXTextureRenderer::CreateDescriptorSets()
{
	descriptor_sets_write_subpass0.resize(swapchain_manager->GetSwapImageCount());

	//subpass0
	{
		//Descriptor sets for vs matrix
		for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
		{
			//ALLOCATE DESCRIPTOR SETS
			VkDescriptorSetAllocateInfo allocInfoWrite{};
			allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfoWrite.descriptorPool     = descriptor_pool;
			allocInfoWrite.descriptorSetCount = 1;
			allocInfoWrite.pSetLayouts        = &descriptor_set_layout_write_subpass0;

			if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &descriptor_sets_write_subpass0[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor sets!");
			}

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			VkWriteDescriptorSet              temp_writeDescriptorSet{};

			//SET INFOS
			/*
				Set 0,Binding 0: VS matrices Uniform buffer
			*/
			VkDescriptorBufferInfo buffer_info_write_subpass0{};
			buffer_info_write_subpass0.buffer = uniform_buffers[i].buffer;
			buffer_info_write_subpass0.offset = 0;
			buffer_info_write_subpass0.range  = sizeof(Ubo_data);

			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_write_subpass0[i];
			temp_writeDescriptorSet.dstBinding      = 0;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.pBufferInfo     = &buffer_info_write_subpass0;
			temp_writeDescriptorSet.dstArrayElement = 0;

			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			/*
				Set 0,Binding 1: FS texture image and sampler
			*/

			VkDescriptorImageInfo image_info_write_subpass0{};
			image_info_write_subpass0.imageView   = ktx_texure.GetTextureImageView();
			image_info_write_subpass0.imageLayout = ktx_texure.GetImageLayout();
			image_info_write_subpass0.sampler     = ktx_texure.GetTextureSampler();

			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_write_subpass0[i];
			temp_writeDescriptorSet.dstBinding      = 1;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			temp_writeDescriptorSet.pImageInfo      = &image_info_write_subpass0;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSets.push_back(temp_writeDescriptorSet);
			//UPDATE DESCRIPTOR SETS
			vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
		}
	}
}

void KTXTextureRenderer::CreateUniformBuffer()
{
	//GPU SIDE
	VkDeviceSize bufferSize = sizeof(Ubo_data);
	uniform_buffers.resize(swapchain_manager->GetSwapImageCount());
	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers[i].buffer, uniform_buffers[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());
		//exclusive mode��Ϊuniform bufferֻ�ᱻgraphics queueʹ��
	}

	////CPU SIDE

	ubo.projection = m_pCamera->GetProj();
	ubo.view       = m_pCamera->GetView();
	ubo.eyepos     = glm::vec4(m_pCamera->GetPosition(), 1.f);
}

void KTXTextureRenderer::CreateDepthImages()
{
	VkFormat depthFormat = swapchain_manager->FindDepthFormat(*device_manager);

	depth_attachment.resize(swapchain_manager->GetSwapImageCount());

	VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager->FindQueueFamilies(device_manager->GetPhysicalDeviceRef(), window->GetSurface());

	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		depth_attachment[i].Init(VK_IMAGE_TYPE_2D, depthFormat, swapchain_manager->GetSwapChainImageExtent(), 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		depth_attachment[i].TransitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue(), queue_family_index);
		depth_attachment[i].InitImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}
}

void KTXTextureRenderer::CreatePiplineSubpass0()
{
	//										 subpass0
	/******************************************************************************************************/
	ShaderManager vertex_shader_subpass0(std::string("../../data/shaders/KTXTexture/KTXTexture_vertex_shader.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_subpass0(std::string("../../data/shaders/KTXTexture/KTXTexture_fragment_shader.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info = {vertex_shader_subpass0.GetVkPipelineShaderStageCreateInfo(), fragment_shader_subpass0.GetVkPipelineShaderStageCreateInfo()};

	//TODO:��Ҫ�����abstraction
	VkVertexInputBindingDescription bindingDescription0{};
	bindingDescription0.binding   = 0;
	bindingDescription0.stride    = sizeof(Vertex);
	bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> VIBDS = {bindingDescription0};

	auto attributeDescriptions = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
	vertex_input_info.pVertexBindingDescriptions      = VIBDS.data();        // Optional
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();        // Optional

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport       viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	viewport.x                            = 0.0f;
	viewport.y                            = (float) extend_of_swap_image.height;
	viewport.width                        = (float) extend_of_swap_image.width;
	viewport.height                       = -(float) extend_of_swap_image.height;
	viewport.minDepth                     = 0.0f;
	viewport.maxDepth                     = 1.0f;

	VkExtent2D swapChainExtent;
	swapChainExtent.height = extend_of_swap_image.height;
	swapChainExtent.width  = extend_of_swap_image.width;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports    = &viewport;
	viewportState.scissorCount  = 1;
	viewportState.pScissors     = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable        = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;

	rasterizer.lineWidth = 1.f;

	rasterizer.cullMode  = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	rasterizer.depthBiasEnable         = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;        // Optional
	rasterizer.depthBiasClamp          = 0.0f;        // Optional
	rasterizer.depthBiasSlopeFactor    = 0.0f;        // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable   = VK_FALSE;
	multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading      = 1.0f;            // Optional
	multisampling.pSampleMask           = nullptr;         // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE;        // Optional
	multisampling.alphaToOneEnable      = VK_FALSE;        // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable         = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable     = VK_FALSE;
	colorBlending.logicOp           = VK_LOGIC_OP_COPY;        // Optional
	colorBlending.attachmentCount   = 1;
	colorBlending.pAttachments      = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;        // Optional
	colorBlending.blendConstants[1] = 0.0f;        // Optional
	colorBlending.blendConstants[2] = 0.0f;        // Optional
	colorBlending.blendConstants[3] = 0.0f;        // Optional

	//VkDynamicState dynamicStates[] = {
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH
	//};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 0;
	dynamicState.pDynamicStates    = VK_NULL_HANDLE;

	VkPipelineDepthStencilStateCreateInfo depthStencil_supass0{};
	depthStencil_supass0.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil_supass0.depthTestEnable       = VK_TRUE;
	depthStencil_supass0.depthWriteEnable      = VK_TRUE;
	depthStencil_supass0.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil_supass0.back.compareOp        = VK_COMPARE_OP_ALWAYS;
	depthStencil_supass0.depthBoundsTestEnable = VK_FALSE;
	depthStencil_supass0.minDepthBounds        = 0.0f;        // Optional
	depthStencil_supass0.maxDepthBounds        = 1.0f;        // Optional
	depthStencil_supass0.stencilTestEnable     = VK_FALSE;
	//depthStencil_supass0.front = {}; // Optional
	//depthStencil_supass0.back = {}; // Optional

	//typedef struct VkGraphicsPipelineCreateInfo {
	//	VkStructureType                                  sType;
	//	const void* pNext;
	//	VkPipelineCreateFlags                            flags;
	//	uint32_t                                         stageCount;
	//	const VkPipelineShaderStageCreateInfo* pStages;
	//	const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
	//	const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
	//	const VkPipelineTessellationStateCreateInfo* pTessellationState;
	//	const VkPipelineViewportStateCreateInfo* pViewportState;
	//	const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
	//	const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
	//	const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
	//	const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
	//	const VkPipelineDynamicStateCreateInfo* pDynamicState;
	//	VkPipelineLayout                                 layout;
	//	VkRenderPass                                     renderPass;
	//	uint32_t                                         subpass;
	//	VkPipeline                                       basePipelineHandle;
	//	int32_t                                          basePipelineIndex;
	//} VkGraphicsPipelineCreateInfo;

	VkGraphicsPipelineCreateInfo pipeline_subpass0_CI{};

	pipeline_subpass0_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_subpass0_CI.stageCount = (uint32_t) shader_stages_create_info.size();
	pipeline_subpass0_CI.pStages    = shader_stages_create_info.data();

	pipeline_subpass0_CI.pVertexInputState   = &vertex_input_info;
	pipeline_subpass0_CI.pInputAssemblyState = &inputAssembly;
	pipeline_subpass0_CI.pViewportState      = &viewportState;
	pipeline_subpass0_CI.pRasterizationState = &rasterizer;
	pipeline_subpass0_CI.pMultisampleState   = &multisampling;
	pipeline_subpass0_CI.pDepthStencilState  = &depthStencil_supass0;

	pipeline_subpass0_CI.pColorBlendState = &colorBlending;
	pipeline_subpass0_CI.pDynamicState    = nullptr;        // Optional

	pipeline_subpass0_CI.layout = pipeline_layout_subpass0;

	pipeline_subpass0_CI.renderPass = render_pass;
	pipeline_subpass0_CI.subpass    = 0;        // index

	pipeline_subpass0_CI.basePipelineIndex  = -1;
	pipeline_subpass0_CI.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline_subpass0_CI, nullptr, &graphics_pipeline_subpass0) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void KTXTextureRenderer::CreateRenderPass()
{
	//swapchain color attachment index 0
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format         = swapchain_manager->GetSwapChainImageFormat();
	colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//Depth attachment index 1
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format         = swapchain_manager->FindDepthFormat(*device_manager);
	depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;        //VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL?
	depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//The index of the attachment ref in this array is directly referenced from the fragment shader with the
	//layout(location = 0) out vec4 outColor directive!

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRefForWrite{};
	depthAttachmentRefForWrite.attachment = 1;
	depthAttachmentRefForWrite.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//======================================================================================================
	VkSubpassDescription subpass0{};
	subpass0.pipelineBindPoint                                             = VK_PIPELINE_BIND_POINT_GRAPHICS;
	std::vector<VkAttachmentReference> WriteColorAttachmentRefsForSubpass0 = {colorAttachmentRef};
	subpass0.colorAttachmentCount                                          = static_cast<uint32_t>(WriteColorAttachmentRefsForSubpass0.size());
	subpass0.pColorAttachments                                             = WriteColorAttachmentRefsForSubpass0.data();
	subpass0.pDepthStencilAttachment                                       = &depthAttachmentRefForWrite;

	//-------------------------------------------------------------------------------------
	std::array<VkSubpassDescription, 1> subpasses = {subpass0};

	std::vector<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};

	//std::array<VkSubpassDependency, 2> dependencies{};

	//dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependencies[0].dstSubpass = 0;

	//dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	////dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	//dependencies[0].srcAccessMask = 0;

	//dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	//dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//dependencies[1].srcSubpass    = 0;        // Last subpass attachment is used in
	//dependencies[1].dstSubpass    = VK_SUBPASS_EXTERNAL;
	//dependencies[1].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	//dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//dependencies[1].dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	//dependencies[1].dstAccessMask   = 0;
	//dependencies[1].dependencyFlags = 0;

	//std::array<VkSubpassDependency, 1> dependencies{};

	//dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependencies[0].dstSubpass = 0;

	//dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	//dependencies[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	//dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	//dependencies[0].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;

	//dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	//renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	//renderPassInfo.pDependencies   = dependencies.data();

	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies   = nullptr;

	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments    = attachments.data();
	renderPassInfo.subpassCount    = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses      = subpasses.data();

	if (vkCreateRenderPass(device_manager->GetLogicalDeviceRef(), &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void KTXTextureRenderer::CreateGraphicsPiplineLayout()
{
	{
		std::vector<VkDescriptorSetLayout> setLayouts = {descriptor_set_layout_write_subpass0};
		VkPipelineLayoutCreateInfo         pipelineLayoutInfo_subpass0{};
		pipelineLayoutInfo_subpass0.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo_subpass0.setLayoutCount = (uint32_t) setLayouts.size();
		pipelineLayoutInfo_subpass0.pSetLayouts    = setLayouts.data();

		//TODO: testing multiple push constants and how to access it

		if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass0, nullptr, &pipeline_layout_subpass0) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
}

void KTXTextureRenderer::CreateGraphicsPipline()
{
	system("..\\..\\data\\shaderbat\\KTXTextureShaderCompile.bat");
	CreatePiplineSubpass0();
}

void KTXTextureRenderer::CreateFramebuffers()
{
	frame_buffers.resize(swapchain_manager->GetSwapImageCount());

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		std::vector<VkImageView> attachments = {
		    swapchain_manager->GetSwapImageViews()[i],
		    depth_attachment[i].GetImageView()};

		auto swap_chain_extent = swapchain_manager->GetSwapChainImageExtent();

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = render_pass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments    = attachments.data();
		framebufferInfo.width           = swap_chain_extent.width;
		framebufferInfo.height          = swap_chain_extent.height;
		framebufferInfo.layers          = 1;        //for 3D application

		if (vkCreateFramebuffer(device_manager->GetLogicalDeviceRef(), &framebufferInfo, nullptr, &frame_buffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void KTXTextureRenderer::InitCommandBuffers()
{
	VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), transfer_command_pool, transfer_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	graphics_command_buffers.resize(swapchain_manager->GetSwapImageCount());

	for (int i = 0; i < graphics_command_buffers.size(); i++)
	{
		VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), graphics_command_pool, graphics_command_buffers[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}
}

void KTXTextureRenderer::CommandBufferRecording()
{
	for (size_t i = 0; i < graphics_command_buffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags            = 0;              // Optional
		beginInfo.pInheritanceInfo = nullptr;        // Optional

		if (vkBeginCommandBuffer(graphics_command_buffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};        //��ʼ��Ϣ���ǣ�ע��
		renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass  = render_pass;
		renderPassInfo.framebuffer = frame_buffers[i];

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = VkExtent2D{swapchain_manager->GetSwapChainImageExtent().width, swapchain_manager->GetSwapChainImageExtent().height};

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color        = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues    = clearValues.data();

		vkCmdBeginRenderPass(graphics_command_buffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);        //

		vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_subpass0);
		vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_subpass0, 0, 1, &descriptor_sets_write_subpass0[i], 0, NULL);

		quad_model->Draw(graphics_command_buffers[i]);

		vkCmdEndRenderPass(graphics_command_buffers[i]);

		if (vkEndCommandBuffer(graphics_command_buffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void KTXTextureRenderer::InitSynObjects()
{
	image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	frame_fences.resize(MAX_FRAMES_IN_FLIGHT);
	image_fences.resize(swapchain_manager->GetSwapImageCount(), VK_NULL_HANDLE);        ///???

	//����ͨ��fence��ѯvkQueueSubmit�Ķ����Ƿ����   vkGetFenceStatus�������Ĳ�ѯ
	//											vkWaitForFences������ѯ��ֱ����������һ�����������е�fence������signaled״̬�����߳�ʱ��ʱ�������ɲ������������Ż᷵�ء�

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
		    vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
		    vkCreateFence(device_manager->GetLogicalDeviceRef(), &fenceInfo, nullptr, &frame_fences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void KTXTextureRenderer::SetUpUserInput()
{
	std::vector<int> tracked_keys = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_Z, GLFW_KEY_C, GLFW_KEY_UP, GLFW_KEY_DOWN};
	keyboard                      = std::make_unique<KeyBoardInputManager>(tracked_keys);
	keyboard->SetupKeyInputs(window->GetWindowPtr());

	mouse = std::make_unique<MouseInputManager>(swapchain_manager->GetSwapChainImageExtent());
	mouse->SetupMouseInputs(window->GetWindowPtr());
}

void KTXTextureRenderer::CreateAttachmentImages()
{
}

void KTXTextureRenderer::DrawFrame()
{
	////imageʹ����Ϻ�subpass dependency�Ĺ�ϵ
	static size_t currentFrame = 0;
	////����fence�ڳ�ʼ��ʱ������signaled��״̬
	////�ȴ�frame
	vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &frame_fences[currentFrame], VK_TRUE, UINT64_MAX);        //vkWaitForFences����ʱ����CPU���ȴ�fence��signal�� �� unsignaled״̬ ��� signaled״̬�Ż�ֹͣ������                  To wait for one or more fences to enter the signaled state on the host,
	uint32_t imageIndex;

	VkResult result = vkAcquireNextImageKHR(device_manager->GetLogicalDeviceRef(), swapchain_manager->GetSwapChain(), UINT64_MAX, image_available_semaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);        //As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.�õ���һ������ʹ�õ�image��index���������image���ܻ�û���꣬�����õ�imageIndex��Ӧ��image���п���������̵�ʱ���ڱ�ĳһ֡ʹ����ϵ���һ������vulkanʵ�־������

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//recreateSwapChain();
		return;
	}
	else if (result == VK_NOT_READY)
	{
		std::cout << ",rely on semophore" << std::endl;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	UpdateUniformBuffer(imageIndex);

	//images_inflight��СΪ3����������ע�����Ĵ�С����2(MAX_FRAMES_IN_FLIGHT)
	if (image_fences[imageIndex] != VK_NULL_HANDLE)
	{
		//	//images_inflight[imageIndex] ���� nullptr��˵��ĳһ֡��GPU��Դ(��image�йص���Դ)���ڱ���imageIndexΪ�±��imageʹ�ã���ô���Ǿ�Ҫ�ȴ���
		//	//���޵ȴ�fence,

		vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &image_fences[imageIndex], VK_TRUE, UINT64_MAX);
	}

	image_fences[imageIndex] = frame_fences[currentFrame];        //�ȴ���images�Ϻ��õ�ǰ��image��currentFrame��ռ��(��ʾcurrentFrame��һ֡��GPU��Դ���ڱ�indexΪimageIndex��imageռ��)��Ŀǰinflight_fences[currentFrame]����signled��״̬��
	////	 inflight_fences[currentFrame]״̬�ı��images_inflight[imageIndex]״̬Ҳ��ı�

	VkSubmitInfo submitInfo{};
	submitInfo.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore          waitSemaphores[] = {image_available_semaphores[currentFrame]};        //�����ǵȴ�imageIndex��Ӧ��image����ռ�õ�GPU��Դ�ͷų�������������Ϊimage����Ҫpresentation�����п���presentation engine��û����ɣ�������Ҫ�ȴ����semaphore
	VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount         = 1;
	submitInfo.pWaitSemaphores            = waitSemaphores;
	submitInfo.pWaitDstStageMask          = waitStages;
	submitInfo.commandBufferCount         = 1;

	//	submitInfo.pCommandBuffers = &graphics_command_buffers[imageIndex];        //�õľ���swap image[imageIndex]
	submitInfo.pCommandBuffers = &graphics_command_buffers[imageIndex];

	VkSemaphore signalSemaphores[]  = {render_finished_semaphores[currentFrame]};        //graphics_command_buffersִ�����Ժ��signal������presentation engine֪����Ⱦ��ɿ���չʾ�ˡ�
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = signalSemaphores;

	////���Կ�������graphics_command_buffers[imageIndex]ʹ�����Ժ�inflight_fences_frame[currentFrame]�ᱻsignal����vkQueueSubmit�Ķ��壩����ô��ôȷ�������graphics_command_buffers[imageIndex]�Ѿ���ǰ��֡ʹ��������أ�

	////������������Ⱦ���̣�

	////===========================================================================================================================================================
	////��һ֡            currentFrame = 0
	////1.����cup�ȴ�   frame_fences[currentFrame 0](��һ��ʹ�ò��õ�)��
	////2.����cup�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ0����ʵ�־���)]������һ��ʹ��ʱ��images_fences[imageIndex 0]Ϊnullptr����
	////3.��ֵ:        images_fences[imageIndex 0������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 0]
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 0]��ʹ��
	////===========================================================================================================================================================
	////�ڶ�֡            currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](��һ���ò��õ�)��
	////2.����cup�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ2����ʵ�־���)]������һ��ʹ��ʱ��images_fences[imageIndex 2]Ϊnullptr����
	////3.��ֵ:        images_fences[imageIndex 2������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1];
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 2]��ʹ��
	////===========================================================================================================================================================

	////����֡����һ�ֿ���) currentFrame = 0
	////1.����cup�ȴ�   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����),
	////2.����cup�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ1����ʵ�־���)]������һ��ʹ��ʱ��images_fences[imageIndex 1]Ϊnullptr����
	////3.��ֵ:        images_fences[imageIndex 1������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 0]
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 1]��ʹ��

	////����֡(�ڶ��ֿ���) currentFrame = 0
	////1.����cup�ȴ�   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����),
	////2.����cup�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ0����ʵ�־���)]��images_fences[0]֮ǰ��frame_fences[currentFrame 0]��ֵ��������ǵȴ�frame_fences[currentFrame 0]����
	////3.��ֵ:        images_fences[imageIndex 0������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 0]
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 0]��ʹ��

	////����֡(�����ֿ���) currentFrame = 0
	////1.����cup�ȴ�   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����),
	////2.����cup�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ2����ʵ�־���)]��images_fences[2]֮ǰ��frame_fences[currentFrame 1]��ֵ��������ǵȴ�frame_fences[currentFrame 1]����
	////3.��ֵ:        images_fences[imageIndex 2������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 0]
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 2]��ʹ��

	////===========================================================================================================================================================

	////#1������Ϊ����֡Ϊ��һ�ֿ��ܣ�
	////����֡����һ�ֿ��ܣ� currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����) ,
	////2.����cpu�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ0����ʵ�־���)]��֮ǰ��frame_fences[currentFrame 0]��ֵ����
	////3.��ֵ:        images_fences[imageIndex 0������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1]��ֵ
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 0]��ʹ����

	////#1������Ϊ����֡Ϊ��һ�ֿ��ܣ�
	////����֡���ڶ��ֿ��ܣ� currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����) ,
	////2.����cpu�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ2����ʵ�־���)]��֮ǰ��frame_fences[currentFrame 1]��ֵ����
	////3.��ֵ:        images_fences[imageIndex 2������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1]��ֵ
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 2]��ʹ����

	////#1������Ϊ����֡Ϊ��һ�ֿ��ܣ�
	////����֡�������ֿ��ܣ� currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����) ,
	////2.����cpu�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ1����ʵ�־���)]��֮ǰ��frame_fences[currentFrame 0]��ֵ����
	////3.��ֵ:        images_fences[imageIndex 1������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1]��ֵ
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 1]��ʹ����

	////===========================================================================================================================================================

	////#1������Ϊ����֡Ϊ�ڶ��ֿ��ܣ�
	////����֡����һ�ֿ��ܣ� currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����) ,
	////2.����cpu�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ0����ʵ�־���)]��images_fences[0]֮ǰ��frame_fences[currentFrame 0]��ֵ����������ǵȴ�frame_fences[currentFrame 0]����
	////3.��ֵ:        images_fences[imageIndex 0������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1]��ֵ
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 0]��ʹ����

	////#1������Ϊ����֡Ϊ�ڶ��ֿ��ܣ�
	////����֡���ڶ��ֿ��ܣ� currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����) ,
	////2.����cpu�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ2����ʵ�־���)]��images_fences[2]֮ǰ��frame_fences[currentFrame 1]��ֵ����������ǵȴ�frame_fences[currentFrame 1]����
	////3.��ֵ:        images_fences[imageIndex 0������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1]��ֵ
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 2]��ʹ����

	////#1������Ϊ����֡Ϊ�ڶ��ֿ��ܣ�
	////����֡���ڶ��ֿ��ܣ� currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����) ,
	////2.����cpu�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ1����ʵ�־���)]������һ��ʹ��ʱ��images_fences[imageIndex 1]Ϊnullptr��
	////3.��ֵ:        images_fences[imageIndex 1������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1]��ֵ
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 1]��ʹ����

	////===========================================================================================================================================================

	////#1������Ϊ����֡Ϊ�����ֿ��ܣ�
	////����֡����һ�ֿ��ܣ� currentFrame = 1
	////1.����cup�ȴ�   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]��Ⱦʹ�ý���������ĵȴ�Ҳ�ͽ�����) ,
	////2.����cpu�ȴ�   images_fences[imageIndex������±��Ǵ�acquireImageindex������ȡ�ģ�����Ϊ0����ʵ�־���)]��֮ǰ��frame_fences[currentFrame 0]��ֵ����������ǵȴ�frame_fences[currentFrame 0]����
	////3.��ֵ:        images_fences[imageIndex 0������±��Ǵ�acquireImageindex��ȡ�ģ�]    =    frame_fences[currentFrame 1]��ֵ
	////4.��Ⱦ:        graphics_command_buffers[imageIndex 0]��ʹ����

	////===========================================================================================================================================================
	//��Ϊ������images_inflight[imageIndex] = inflight_fences[currentFrame]; ������ʱ��images_inflight[imageIndex]�� inflight_fences[currentFrame]����ͬ����״̬;����Ӧ��ͬʱ������unsignaled״̬
	vkResetFences(device_manager->GetLogicalDeviceRef(), 1, &frame_fences[currentFrame]);        //To set the state of fences to unsignaled from the host side
	//vkQueueSubmit:   fence(last parameter is an optional handle to a fence to be signaled once all submitted command buffers have completed execution. If fence is not VK_NULL_HANDLE, it defines a fence signal operation.��command buffer�е�����ִ�н����Ժ�
	if (vkQueueSubmit(device_manager->GetGraphicsQueue(), 1, &submitInfo, frame_fences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapchain_manager->GetSwapChain()};
	presentInfo.pSwapchains     = swapChains;
	presentInfo.swapchainCount  = 1;
	presentInfo.pImageIndices   = &imageIndex;
	presentInfo.pResults        = nullptr;        // Optional

	result = vkQueuePresentKHR(device_manager->GetPresentQueue(), &presentInfo);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
	//	framebufferResized = false;
	//	//recreateSwapChain();

	//}

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		//recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void KTXTextureRenderer::UpdateUniformBuffer(uint32_t currentImage)
{
	ubo.projection = m_pCamera->GetProj();
	ubo.view       = m_pCamera->GetView();
	ubo.eyepos     = glm::vec4(m_pCamera->GetPosition(), 1.f);

	void *data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers[currentImage].memory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(Ubo_data));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers[currentImage].memory);
}

void KTXTextureRenderer::UpdateCamera(float dt)
{
	if (keyboard->GetIsKeyDown(GLFW_KEY_W))
	{
		m_pCamera->Walk(dt * -3.f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_S))
	{
		m_pCamera->Walk(dt * 3.f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_A))
	{
		m_pCamera->Strafe(dt * -3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_D))
	{
		m_pCamera->Strafe(dt * 3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_UP))
	{
		m_pCamera->MoveForward(dt * -3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_DOWN))
	{
		m_pCamera->MoveForward(dt * 3.0f);
	}
	//=====================================================================

	m_pCamera->Pitch(dt * mouse->GetPitchDiff());
	m_pCamera->RotateY(-dt * mouse->GetYawDiff());
}

void KTXTextureRenderer::CreateCamera()
{
	m_pCamera = std::make_unique<FirstPersonCamera>();
	m_pCamera->SetFrustum(glm::radians(60.f), swapchain_manager->GetSwapChainImageExtent().width / (float) swapchain_manager->GetSwapChainImageExtent().height, 0.1f, 256.f);
	m_pCamera->LookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0., 1., 0.));

	VkViewport       viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	viewport.x                            = 0.0f;
	viewport.y                            = (float) extend_of_swap_image.height;
	viewport.width                        = (float) extend_of_swap_image.width;
	viewport.height                       = -(float) extend_of_swap_image.height;
	viewport.minDepth                     = 0.0f;
	viewport.maxDepth                     = 1.0f;

	m_pCamera->SetViewPort(viewport);
}

void KTXTextureRenderer::CleanUpModels()
{
	quad_model->CleanUp();
}

void KTXTextureRenderer::CleanUpDescriptorSetLayoutAndDescriptorPool()
{
	vkDestroyDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), descriptor_set_layout_write_subpass0, nullptr);
	vkDestroyDescriptorPool(device_manager->GetLogicalDeviceRef(), descriptor_pool, nullptr);


}

void KTXTextureRenderer::CleanUpCommandBuffersAndCommandPool()
{
	vkFreeCommandBuffers(device_manager->GetLogicalDeviceRef(), graphics_command_pool, static_cast<uint32_t>(graphics_command_buffers.size()), graphics_command_buffers.data());
	vkFreeCommandBuffers(device_manager->GetLogicalDeviceRef(), transfer_command_pool, 1, &transfer_command_buffer);

	vkDestroyCommandPool(device_manager->GetLogicalDeviceRef(), graphics_command_pool, nullptr);
	vkDestroyCommandPool(device_manager->GetLogicalDeviceRef(), transfer_command_pool, nullptr);
}

void KTXTextureRenderer::CleanUpSyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device_manager->GetLogicalDeviceRef(), render_finished_semaphores[i], nullptr);
		vkDestroySemaphore(device_manager->GetLogicalDeviceRef(), image_available_semaphores[i], nullptr);
		vkDestroyFence(device_manager->GetLogicalDeviceRef(), frame_fences[i], nullptr);
	}



}

void KTXTextureRenderer::CleanupFrameBuffers()
{
	for (auto framebuffer : frame_buffers)
	{
		vkDestroyFramebuffer(device_manager->GetLogicalDeviceRef(), framebuffer, nullptr);
	}
}

void KTXTextureRenderer::CleanUpPiplineAndPiplineLayout()
{
	vkDestroyPipeline(device_manager->GetLogicalDeviceRef(), graphics_pipeline_subpass0, nullptr);
	vkDestroyPipelineLayout(device_manager->GetLogicalDeviceRef(), pipeline_layout_subpass0, nullptr);
}

void KTXTextureRenderer::CleanUpRenderPass()
{
	vkDestroyRenderPass(device_manager->GetLogicalDeviceRef(), render_pass, nullptr);
}

void KTXTextureRenderer::CleanUpImages()
{
	for (auto &var : depth_attachment)
	{
		var.CleanUp();
	}
	ktx_texure.CleanUp();
}

void KTXTextureRenderer::CleanUpUniformBuffers()
{
}
