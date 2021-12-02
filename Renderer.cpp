#include "Renderer.h"

void Renderer::SetGraphicsCommandPool(VkCommandPool commandpool)
{



	graphics_command_pool = commandpool;
	return;
}

void Renderer::SetTransforCommandPool(VkCommandPool commandpool)
{
	transfor_command_pool = commandpool;
	return;
}


void Renderer::SetSwapChinManager(VkSwapChainManager& para_swapchain_manager)
{

	swapchain_manager = &para_swapchain_manager;
}

void Renderer::PrepareModels()
{


	std::vector<InsatnceTranformation> instanceData;
	std::vector<Vertex> vertices;
	vertices.resize(4);
	vertices[0] = Vertex{ glm::vec3(-1.f,0.f,-1.f), glm::vec3(1.f,0.f,1.f)   ,glm::vec2(1.f,0.f) };
	vertices[1] = Vertex{ glm::vec3(1.f,0.f,-1.f), glm::vec3(1.f,0.f,1.f)    ,glm::vec2(1.f,1.f) };
	vertices[2] = Vertex{ glm::vec3(1.f,0.f, 1.f), glm::vec3(1.f,0.f,1.f)    ,glm::vec2(0.f,1.f) };
	vertices[3] = Vertex{ glm::vec3(-1.f,0.f, 1.f), glm::vec3(1.f,0.f,1.f)   ,glm::vec2(0.f,0.f) };
	std::vector<uint16_t> indices = { 0,1,2,0,2,3 };



	std::default_random_engine randomEngine{ 15u };
	std::uniform_real_distribution<float> myUnifFlaotDist{ -5.f, 5.f };

	auto numOfInstance = 7;

	instanceData.resize(numOfInstance);
	for (int i = 0; i < numOfInstance; i++) {
		float xDis = myUnifFlaotDist(randomEngine);
		float yDis = myUnifFlaotDist(randomEngine);
		float zDis = myUnifFlaotDist(randomEngine);

		float angleAroundX = myUnifFlaotDist(randomEngine);
		glm::mat4 transforRoate = glm::mat4(1.0f);
		transforRoate = glm::rotate(transforRoate, angleAroundX, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 transforDisplacement = glm::mat4(1.0f);
		transforDisplacement = glm::translate(transforDisplacement, glm::vec3(xDis, yDis, -5.f));
		instanceData[i].worldTrans = transforDisplacement * transforRoate;
	}


	scene.emplace_back(vertices, instanceData,indices, device_manager, window->GetSurface(), transfer_command_buffer);


//========================================================================================================================











}

void Renderer::SetDeviceManager(VkDeviceManager& para_device_manager)
{
	device_manager = &para_device_manager;
}

void Renderer::SetWindow(VkWindows& para_window)
{
	window = &para_window;
}





void Renderer::CreateDescriptorPool()
{




	// Create the global descriptor pool
	std::array<VkDescriptorPoolSize, 3> poolSizes{};



	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 6;  // 6


	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 3;  //3

	poolSizes[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	poolSizes[2].descriptorCount = 6;  //6


	VkDescriptorPoolCreateInfo descriptorPoolCI = {};
	descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCI.pPoolSizes = poolSizes.data();
	// Max. number of descriptor sets that can be allocated from this pool (one per object)
	descriptorPoolCI.maxSets = static_cast<uint32_t>(swapchain_manager->GetSwapImageCount() * 2);




	if (vkCreateDescriptorPool(device_manager->GetLogicalDeviceRef(), &descriptorPoolCI, nullptr, &descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}




}

void Renderer::CreateDescriptorSets()
{



	descriptor_sets_write_subpass0.resize(swapchain_manager->GetSwapImageCount());
	descriptor_sets_read_subpass1.resize(swapchain_manager->GetSwapImageCount());


	//subpass0 writing

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {

		VkDescriptorSetAllocateInfo allocInfoWrite{};
		allocInfoWrite.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfoWrite.descriptorPool = descriptor_pool;
		allocInfoWrite.descriptorSetCount = 1;
		allocInfoWrite.pSetLayouts = &descriptor_set_layout_write_subpass0;

		if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &descriptor_sets_write_subpass0[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}


		VkDescriptorBufferInfo buffer_info_write_subpass0{};
		buffer_info_write_subpass0.buffer = uniform_buffers[i].buffer;
		buffer_info_write_subpass0.offset = 0;
		buffer_info_write_subpass0.range = sizeof(UniformBufferObject);


		VkDescriptorBufferInfo buffer_info_write_subpass0_test{};
		buffer_info_write_subpass0_test.buffer = uniform_buffers_test[i].buffer;
		buffer_info_write_subpass0_test.offset = 0;
		buffer_info_write_subpass0_test.range = sizeof(UniformBufferObjectTest);



		//std::vector< VkDescriptorBufferInfo> bufferInfos = { buffer_info_write_subpass0 ,buffer_info_write_subpass0_test };




		VkDescriptorImageInfo imageInfoForTexureLoaded{};
		imageInfoForTexureLoaded.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForTexureLoaded.imageView = human_face.GetTextureImageView();
		imageInfoForTexureLoaded.sampler = human_face.GetTextureSampler();


		std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};

		/*
			Binding 0: Object matrices Uniform buffer
		*/
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = descriptor_sets_write_subpass0[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		writeDescriptorSets[0].pBufferInfo = &buffer_info_write_subpass0;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].dstArrayElement = 0;


		/*
			Binding 1: test purposes uniform buffer
		*/

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = descriptor_sets_write_subpass0[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// Images use a different descriptor structure, so we use pImageInfo instead of pBufferInfo
		writeDescriptorSets[1].pBufferInfo = &buffer_info_write_subpass0_test;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].dstArrayElement = 0;



		/*
			Binding 2: texture mapping
		*/
		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].dstSet = descriptor_sets_write_subpass0[i];
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[2].pImageInfo = &imageInfoForTexureLoaded;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].dstArrayElement = 0;

		vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}






	//subpass1 reading

	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {


		VkDescriptorSetAllocateInfo allocInfoRead{};
		allocInfoRead.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfoRead.descriptorPool = descriptor_pool;
		allocInfoRead.descriptorSetCount = 1;
		allocInfoRead.pSetLayouts = &descriptor_set_layout_read_subpass1;



		if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoRead, &descriptor_sets_read_subpass1[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}


		std::array<VkDescriptorImageInfo, 2> imageInfoForSubPass1{};
		imageInfoForSubPass1[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForSubPass1[0].imageView = red_color_attachment[i].GetImageView();
		imageInfoForSubPass1[0].sampler = VK_NULL_HANDLE;

		imageInfoForSubPass1[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForSubPass1[1].imageView = depth_attachment[i].GetImageView();
		imageInfoForSubPass1[1].sampler = VK_NULL_HANDLE;



		std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};

		/*
			Binding 0: Rcolor
		*/
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = descriptor_sets_read_subpass1[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[0].pImageInfo = &imageInfoForSubPass1[0];
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].pBufferInfo = VK_NULL_HANDLE;




		/*
			Binding 2: depthValue
		*/
		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = descriptor_sets_read_subpass1[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[1].pImageInfo = &imageInfoForSubPass1[1];
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].pBufferInfo = VK_NULL_HANDLE;



		vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);


	}












}

void Renderer::CreateUniformBuffer()
{


	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	uniform_buffers.resize(swapchain_manager->GetSwapImageCount());
	/*uniform_buffers_memory.resize(swapchain_manager->GetSwapImageCount());*/

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers[i].buffer, uniform_buffers[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}



	VkDeviceSize bufferSizeTest = sizeof(UniformBufferObjectTest);
	uniform_buffers_test.resize(swapchain_manager->GetSwapImageCount());


	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {
		device_manager->CreateBuffer(bufferSizeTest, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_test[i].buffer, uniform_buffers_test[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}


}

void Renderer::CreateDepthImages()
{
	VkFormat depthFormat = swapchain_manager->FindDepthFormat(*device_manager);


	depth_attachment.resize(swapchain_manager->GetSwapImageCount());

	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {
		depth_attachment[i].Init(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, swapchain_manager->GetSwapChainImageExtent(), 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		depth_attachment[i].TransitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue());
		depth_attachment[i].InitImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}



}



void Renderer::CreatePiplineSubpass0()
{










	//										 subpass0
	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	ShaderManager vertex_shader_subpass0(std::string("vert.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_subpass0(std::string("frag.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());






	/*auto vertShaderCode = ShaderManager::ReadFile("vert.spv");
	auto fragShaderCode = ShaderManager::ReadFile("frag.spv");

	VkShaderModule vertShaderModule = ShaderManager::CreateShaderModule(vertShaderCode,device_manager->GetLogicalDeviceRef());
	VkShaderModule fragShaderModule = ShaderManager::CreateShaderModule(fragShaderCode, device_manager->GetLogicalDeviceRef());


	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";


	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";*/



	VkPipelineShaderStageCreateInfo shader_stages_create_info[] = { vertex_shader_subpass0.GetVkPipelineShaderStageCreateInfo(), fragment_shader_subpass0.GetVkPipelineShaderStageCreateInfo() };


	//auto bindingDescription = Vertex::getBindingDescription();
	//auto attributeDescriptions = Vertex::getAttributeDescriptions();












	//TODO:需要更多的abstraction
	VkVertexInputBindingDescription bindingDescription0{};

	bindingDescription0.binding = 0;
	bindingDescription0.stride = sizeof(Vertex);
	bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;



	VkVertexInputBindingDescription bindingDescription1{};
	bindingDescription1.binding = 1;
	bindingDescription1.stride = sizeof(InsatnceTranformation);
	bindingDescription1.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	std::vector<VkVertexInputBindingDescription>  VIBDS = { bindingDescription0 ,bindingDescription1 };



	std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);


	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);





	attributeDescriptions[3].binding = 1;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[3].offset = sizeof(float) * 4 * 0;

	attributeDescriptions[4].binding = 1;
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[4].offset = sizeof(float) * 4 * 1;


	attributeDescriptions[5].binding = 1;
	attributeDescriptions[5].location = 5;
	attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[5].offset = sizeof(float) * 4 * 2;

	attributeDescriptions[6].binding = 1;
	attributeDescriptions[6].location = 6;
	attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[6].offset = sizeof(float) * 4 * 3;





	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = (uint32_t)VIBDS.size();
	vertex_input_info.pVertexBindingDescriptions = VIBDS.data(); // Optional
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional



















	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkViewport viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	viewport.x = 0.0f;
	viewport.y = (float)extend_of_swap_image.height;
	viewport.width = (float)extend_of_swap_image.width;
	viewport.height = -(float)extend_of_swap_image.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkExtent2D swapChainExtent;
	swapChainExtent.height = extend_of_swap_image.height;
	swapChainExtent.width = extend_of_swap_image.width;


	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;



	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

	rasterizer.lineWidth = 1.f;

	rasterizer.cullMode = VK_CULL_MODE_NONE;
	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;


	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional



	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional


	//VkDynamicState dynamicStates[] = {
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH
	//};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 0;
	dynamicState.pDynamicStates = VK_NULL_HANDLE;

	VkPipelineDepthStencilStateCreateInfo depthStencil_supass0{};
	depthStencil_supass0.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil_supass0.depthTestEnable = VK_TRUE;
	depthStencil_supass0.depthWriteEnable = VK_TRUE;
	depthStencil_supass0.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil_supass0.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depthStencil_supass0.depthBoundsTestEnable = VK_FALSE;
	depthStencil_supass0.minDepthBounds = 0.0f; // Optional
	depthStencil_supass0.maxDepthBounds = 1.0f; // Optional
	depthStencil_supass0.stencilTestEnable = VK_FALSE;
	depthStencil_supass0.front = {}; // Optional
	depthStencil_supass0.back = {}; // Optional




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


	pipeline_subpass0_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_subpass0_CI.stageCount = 2;
	pipeline_subpass0_CI.pStages = shader_stages_create_info;




	pipeline_subpass0_CI.pVertexInputState = &vertex_input_info;
	pipeline_subpass0_CI.pInputAssemblyState = &inputAssembly;
	pipeline_subpass0_CI.pViewportState = &viewportState;
	pipeline_subpass0_CI.pRasterizationState = &rasterizer;
	pipeline_subpass0_CI.pMultisampleState = &multisampling;
	pipeline_subpass0_CI.pDepthStencilState = &depthStencil_supass0;

	pipeline_subpass0_CI.pColorBlendState = &colorBlending;
	pipeline_subpass0_CI.pDynamicState = nullptr; // Optional

	pipeline_subpass0_CI.layout = pipeline_layout_subpass0;

	pipeline_subpass0_CI.renderPass = render_pass;
	pipeline_subpass0_CI.subpass = 0;// index



	pipeline_subpass0_CI.basePipelineIndex = -1;
	pipeline_subpass0_CI.basePipelineHandle = VK_NULL_HANDLE;



	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline_subpass0_CI, nullptr, &graphics_pipeline_subpass0) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}














}

void Renderer::CreatePiplineSubpass1()
{








	/******************************************************************************************************/
	ShaderManager vertex_shader_subpass1(std::string("vert0.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_subpass1(std::string("frag0.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());




	VkPipelineShaderStageCreateInfo shader_stages_create_info[] = { vertex_shader_subpass1.GetVkPipelineShaderStageCreateInfo(), fragment_shader_subpass1.GetVkPipelineShaderStageCreateInfo() };







	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;



	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;






	VkPipelineDepthStencilStateCreateInfo depthStencil_supass1{};
	depthStencil_supass1.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil_supass1.depthTestEnable = VK_TRUE;
	depthStencil_supass1.depthWriteEnable = VK_FALSE;
	depthStencil_supass1.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil_supass1.depthBoundsTestEnable = VK_FALSE;
	depthStencil_supass1.minDepthBounds = 0.0f; // Optional
	depthStencil_supass1.maxDepthBounds = 1.0f; // Optional
	depthStencil_supass1.stencilTestEnable = VK_FALSE;
	depthStencil_supass1.front = {}; // Optional
	depthStencil_supass1.back = {}; // Optional






	VkPipelineVertexInputStateCreateInfo empty_input_stateCI{};
	empty_input_stateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;











	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional











	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional




	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = (float)extend_of_swap_image.height;
	viewport.width = (float)extend_of_swap_image.width;
	viewport.height = -(float)extend_of_swap_image.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkExtent2D swapChainExtent;
	swapChainExtent.height = extend_of_swap_image.height;
	swapChainExtent.width = extend_of_swap_image.width;


	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;


	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;



	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional







	VkGraphicsPipelineCreateInfo pipeline_subpass1_CI{};


	pipeline_subpass1_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_subpass1_CI.stageCount = 2;
	pipeline_subpass1_CI.pStages = shader_stages_create_info;




	pipeline_subpass1_CI.pVertexInputState = &empty_input_stateCI;
	pipeline_subpass1_CI.pInputAssemblyState = &inputAssembly;
	pipeline_subpass1_CI.pViewportState = &viewportState;
	pipeline_subpass1_CI.pRasterizationState = &rasterizer;
	pipeline_subpass1_CI.pMultisampleState = &multisampling;
	pipeline_subpass1_CI.pDepthStencilState = &depthStencil_supass1;

	pipeline_subpass1_CI.pColorBlendState = &colorBlending;
	pipeline_subpass1_CI.pDynamicState = nullptr; // Optional

	pipeline_subpass1_CI.layout = pipeline_layout_subpass1;




	pipeline_subpass1_CI.renderPass = render_pass;
	pipeline_subpass1_CI.subpass = 1;// index



	pipeline_subpass1_CI.basePipelineIndex = -1;
	pipeline_subpass1_CI.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline_subpass1_CI, nullptr, &graphics_pipeline_subpass1) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}




































}



void Renderer::CreateRenderPass()
{


	//swapchain color index 0
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchain_manager->GetSwapChainImageFormat();                //
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	//R color index 1
	VkAttachmentDescription RchannelcolorAttachment{};
	RchannelcolorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;            //这里的format是否和上面的一致？
	RchannelcolorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	RchannelcolorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	RchannelcolorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	RchannelcolorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	RchannelcolorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	RchannelcolorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	RchannelcolorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Depth index 2
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = swapchain_manager->FindDepthFormat(*device_manager);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;








	//The index of the attachment ref in this array is directly referenced from the fragment shader with the 
	//layout(location = 0) out vec4 outColor directive!


	//subpass0用的
	VkAttachmentReference RAttachmentRef{};
	RAttachmentRef.attachment = 1;
	RAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRefForWrite{};
	depthAttachmentRefForWrite.attachment = 2;
	depthAttachmentRefForWrite.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



	//subpass1用的
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference RAttachmentRefForRead{};
	RAttachmentRefForRead.attachment = 1;
	RAttachmentRefForRead.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRefForRead{};
	depthAttachmentRefForRead.attachment = 2;
	depthAttachmentRefForRead.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;




	std::vector<VkAttachmentReference> WriteColorAttachmentRefsForSubpass0 = { RAttachmentRef };
	VkSubpassDescription subpass0{};
	subpass0.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass0.colorAttachmentCount = static_cast<uint32_t>(WriteColorAttachmentRefsForSubpass0.size());
	subpass0.pColorAttachments = WriteColorAttachmentRefsForSubpass0.data();
	subpass0.pDepthStencilAttachment = &depthAttachmentRefForWrite;



	std::vector<VkAttachmentReference> outPutColorAttachmentRefsForSubpass1 = { colorAttachmentRef };
	std::vector<VkAttachmentReference> inPutAttachmentRefsForSubpass1 = { RAttachmentRefForRead ,depthAttachmentRefForRead };



	VkSubpassDescription subpass1{};
	subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass1.colorAttachmentCount = static_cast<uint32_t>(outPutColorAttachmentRefsForSubpass1.size());
	subpass1.pColorAttachments = outPutColorAttachmentRefsForSubpass1.data();
	subpass1.inputAttachmentCount = static_cast<uint32_t>(inPutAttachmentRefsForSubpass1.size());
	subpass1.pInputAttachments = inPutAttachmentRefsForSubpass1.data();
	subpass1.pDepthStencilAttachment = VK_NULL_HANDLE;


	std::array<VkSubpassDescription, 2> subpasses = { subpass0 ,subpass1 };

	std::vector<VkAttachmentDescription> attachments = { colorAttachment,RchannelcolorAttachment,depthAttachment };



	std::array<VkSubpassDependency, 3> dependencies{};

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;


	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	//dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].srcAccessMask = 0;


	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// This dependency transitions the input attachment from color attachment to shader read only layout
	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = 1;

	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	dependencies[2].srcSubpass = 1;        // Last subpass attachment is used in
	dependencies[2].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[2].srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	dependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	dependencies[2].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[2].dstAccessMask = 0;
	dependencies[2].dependencyFlags = 0;







	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();


	if (vkCreateRenderPass(device_manager->GetLogicalDeviceRef(), &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}





}

void Renderer::CreateDescriptorSetLayout()
{

	/*
			Attachment write
	*/

	std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass0;

	LayoutBindingSubpass0.resize(3, VkDescriptorSetLayoutBinding{});
	LayoutBindingSubpass0[0].binding = 0; //index  uniform buffer
	LayoutBindingSubpass0[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LayoutBindingSubpass0[0].descriptorCount = 1;
	LayoutBindingSubpass0[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	LayoutBindingSubpass0[0].pImmutableSamplers = nullptr; // Optional



	LayoutBindingSubpass0[1].binding = 1; //index  testing purposes uniform buffer
	LayoutBindingSubpass0[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LayoutBindingSubpass0[1].descriptorCount = 1;
	LayoutBindingSubpass0[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	LayoutBindingSubpass0[1].pImmutableSamplers = nullptr; // Optional



	LayoutBindingSubpass0[2].binding = 2; //index  texture mapping 
	LayoutBindingSubpass0[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	LayoutBindingSubpass0[2].descriptorCount = 1;
	LayoutBindingSubpass0[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBindingSubpass0[2].pImmutableSamplers = nullptr; // Optional







	VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass0{};
	LayoutBindingCISubpass0.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutBindingCISubpass0.bindingCount = (uint32_t)LayoutBindingSubpass0.size();  //the amount of VkDescriptorSetLayoutBinding
	LayoutBindingCISubpass0.pBindings = LayoutBindingSubpass0.data();


	if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass0, nullptr, &descriptor_set_layout_write_subpass0) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}






	/*
		Attachment read
	*/

	std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass1;
	LayoutBindingSubpass1.resize(3, VkDescriptorSetLayoutBinding{});


	LayoutBindingSubpass1[0].binding = 0; //index  Rcolor read
	LayoutBindingSubpass1[0].descriptorCount = 1;
	LayoutBindingSubpass1[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	LayoutBindingSubpass1[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBindingSubpass1[0].pImmutableSamplers = nullptr; // Optional



	LayoutBindingSubpass1[2].binding = 1;// depth value read
	LayoutBindingSubpass1[2].descriptorCount = 1;
	LayoutBindingSubpass1[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	LayoutBindingSubpass1[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBindingSubpass1[2].pImmutableSamplers = nullptr; // Optional



	VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass1{};
	LayoutBindingCISubpass1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutBindingCISubpass1.bindingCount = (uint32_t)LayoutBindingSubpass1.size();  //the amount of VkDescriptorSetLayoutBinding
	LayoutBindingCISubpass1.pBindings = LayoutBindingSubpass1.data();


	if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass1, nullptr, &descriptor_set_layout_read_subpass1) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}



}

void Renderer::CreateGraphicsPipline()
{

	system("CompileShader.bat");


	CreatePiplineSubpass0();
	CreatePiplineSubpass1();




}

void Renderer::CreateFramebuffers()
{



	frame_buffers.resize(swapchain_manager->GetSwapImageCount());

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {

		std::vector<VkImageView> attachments = {
			swapchain_manager->GetSwapImageViews()[i],
			red_color_attachment[i].GetImageView(),
			depth_attachment[i].GetImageView()
		};

		auto swap_chain_extent = swapchain_manager->GetSwapChainImageExtent();


		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = render_pass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swap_chain_extent.width;
		framebufferInfo.height = swap_chain_extent.height;
		framebufferInfo.layers = 1;//for 3D application

		if (vkCreateFramebuffer(device_manager->GetLogicalDeviceRef(), &framebufferInfo, nullptr, &frame_buffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}


}

void Renderer::InitCommandBuffers()
{


	VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), transfor_command_pool, transfer_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	graphics_command_buffers.resize(swapchain_manager->GetSwapImageCount());


	for (int i = 0; i < graphics_command_buffers.size(); i++) {

		VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), graphics_command_pool, graphics_command_buffers[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	}




}

void Renderer::CommandBufferRecording()
{








	for (size_t i = 0; i < graphics_command_buffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional
		if (vkBeginCommandBuffer(graphics_command_buffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}








		VkRenderPassBeginInfo renderPassInfo{}; //开始信息这是，注意
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		//renderPassInfo.renderPass = render_pass;
		renderPassInfo.renderPass = render_pass;


		renderPassInfo.framebuffer = frame_buffers[i];


		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = VkExtent2D{ swapchain_manager->GetSwapChainImageExtent().width , swapchain_manager->GetSwapChainImageExtent().height };

		std::array<VkClearValue, 3> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[2].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();





		vkCmdBeginRenderPass(graphics_command_buffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//

		///*
		//		First sub pass
		//		Fills the attachments
		//*/

		vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_subpass0);
		vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_subpass0, 0, 1, &descriptor_sets_write_subpass0[i], 0, NULL);


		for (auto& model : scene) {

			model.DrawInstance(graphics_command_buffers[i]);
		}



		///*
		//	Second sub pass
		//	Render a full screen quad, reading from the previously written attachments via input attachments
		//*/
		vkCmdNextSubpass(graphics_command_buffers[i], VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_subpass1);
		vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_subpass1, 0, 1, &descriptor_sets_read_subpass1[i], 0, NULL);



		vkCmdDraw(graphics_command_buffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(graphics_command_buffers[i]);


		if (vkEndCommandBuffer(graphics_command_buffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}



	}




}

void Renderer::InitSynObjects()
{







	image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inflight_fences.resize(MAX_FRAMES_IN_FLIGHT);
	images_inflight.resize(swapchain_manager->GetSwapImageCount(), VK_NULL_HANDLE); ///???

	//可以通过fence查询vkQueueSubmit的动作是否完成   vkGetFenceStatus非阻塞的查询   
	//											   vkWaitForFences阻塞查询，直到其中至少一个，或者所有的fence都处于signaled状态，或者超时（时间限制由参数给出），才会返回。


	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device_manager->GetLogicalDeviceRef(), &fenceInfo, nullptr, &inflight_fences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}



}



void Renderer::CleanUpModels()
{
}

void Renderer::DrawFrame()
{


	static 	size_t currentFrame = 0;

	vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &inflight_fences[currentFrame], VK_TRUE, UINT64_MAX); //vkWaitForFences无限时阻塞CPU，等待fence被signal后 从 unsignaled状态 变成 signaled状态 这里应该是防止和自己(currentFrame)冲突。To wait for one or more fences to enter the signaled state on the host,




	uint32_t imageIndex;


	VkResult result = vkAcquireNextImageKHR(device_manager->GetLogicalDeviceRef(), swapchain_manager->GetSwapChain(), UINT64_MAX, image_available_semaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);//As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.得到下一个可以使用的image的index，但是这个image可能还没用完，这里获得的imageIndex对应的image很有可能是最快被某一帧使用完毕的那一个，由vulkan实现具体决定


	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		//recreateSwapChain();
		return;
	}
	else if (result == VK_NOT_READY) {
		std::cout << "FUck you mother fucker,image of index %d is not ready!!,rely on semophore" << std::endl;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}





	UpdateUniformBuffer(imageIndex);

	//images_inflight大小为3在这个例子里，注意它的大小不是MAX_FRAMES_IN_FLIGHT 
	if (images_inflight[imageIndex] != VK_NULL_HANDLE) { //images_inflight[imageIndex] 不是 nullptr，说明某一帧的GPU资源正在被以imageIndex为下标的image使用，那么我们就要等待。
		vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &images_inflight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	images_inflight[imageIndex] = inflight_fences[currentFrame]; //等待完毕后的images，被我们的currentFrame所占有(表示currentFrame这一帧的GPU资源正在被index为imageIndex的image占用)，目前inflight_fences[currentFrame]处于signled的状态。
	//	 inflight_fences[currentFrame]状态改变后，images_inflight[imageIndex]状态也会改变




	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { image_available_semaphores[currentFrame] };     //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image还需要presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &graphics_command_buffers[imageIndex];
	VkSemaphore signalSemaphores[] = { render_finished_semaphores[currentFrame] };  //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;





	//因为上面有images_inflight[imageIndex] = inflight_fences[currentFrame]; 所以这时候images_inflight[imageIndex]和 inflight_fences[currentFrame];应该同时进入了unsignaled状态
	vkResetFences(device_manager->GetLogicalDeviceRef(), 1, &inflight_fences[currentFrame]); //To set the state of fences to unsignaled from the host





	//VkResult A = vkQueueSubmit(device_manager->GetGraphicsQueue(), 1, &submitInfo, inflight_fences[currentFrame]);

	if (vkQueueSubmit(device_manager->GetGraphicsQueue(), 1, &submitInfo, inflight_fences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//vkQueueSubmit(device_manager->GetGraphicsQueue(), 1, &submitInfo, inflight_fences[currentFrame]);
	//fence is an optional handle to a fence to be signaled once all submitted command buffers have completed execution.If fence is not VK_NULL_HANDLE, it defines a fence signal operation.





	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchain_manager->GetSwapChain() };
	presentInfo.pSwapchains = swapChains;
	presentInfo.swapchainCount = 1;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional



	result = vkQueuePresentKHR(device_manager->GetPresentQueue(), &presentInfo);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
	//	framebufferResized = false;
	//	//recreateSwapChain();

	//}

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ) {
		//recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


}

void Renderer::UpdateUniformBuffer(uint32_t currentImage)
{



	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();




	UniformBufferObject ubo{};
	ubo.model = glm::mat4(1.0f);
	ubo.view = glm::lookAtRH(glm::vec3(0.f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspectiveRH_ZO(glm::radians(90.f), swapchain_manager->GetSwapChainImageExtent().width / (float)swapchain_manager->GetSwapChainImageExtent().height, 3.f, 9.0f);





	UniformBufferObjectTest ubo0{};
	ubo0.model = glm::mat4(1.0f);
	ubo0.view = glm::lookAtRH(glm::vec3(0.f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));



	void* data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers[currentImage].memory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(UniformBufferObject));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers[currentImage].memory);


	void* data0;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_test[currentImage].memory, 0, sizeof(ubo0), 0, &data0);
	memcpy(data0, &ubo0, sizeof(UniformBufferObjectTest));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_test[currentImage].memory);



}



void Renderer::CleanUpDescriptorSetLayoutAndDescriptorPool()
{


	vkDestroyDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), descriptor_set_layout_write_subpass0, nullptr);
	vkDestroyDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), descriptor_set_layout_read_subpass1, nullptr);


	vkDestroyDescriptorPool(device_manager->GetLogicalDeviceRef(), descriptor_pool, nullptr);


}

void Renderer::CleanUpCommandBuffersAndCommandPool()
{

	vkFreeCommandBuffers(device_manager->GetLogicalDeviceRef(), graphics_command_pool, static_cast<uint32_t>(graphics_command_buffers.size()), graphics_command_buffers.data());
	vkFreeCommandBuffers(device_manager->GetLogicalDeviceRef(), transfor_command_pool, 1, &transfer_command_buffer);



	vkDestroyCommandPool(device_manager->GetLogicalDeviceRef(), graphics_command_pool, nullptr);
	vkDestroyCommandPool(device_manager->GetLogicalDeviceRef(), transfor_command_pool, nullptr);

}

void Renderer::CleanUpSyncObjects()
{

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device_manager->GetLogicalDeviceRef(), render_finished_semaphores[i], nullptr);
		vkDestroySemaphore(device_manager->GetLogicalDeviceRef(), image_available_semaphores[i], nullptr);
		vkDestroyFence(device_manager->GetLogicalDeviceRef(), inflight_fences[i], nullptr);
	}

}

void Renderer::CleanupFrameBuffers()
{


	for (auto framebuffer : frame_buffers) {
		vkDestroyFramebuffer(device_manager->GetLogicalDeviceRef(), framebuffer, nullptr);
	}

}

void Renderer::CleanUpPiplineAndPiplineLayout()
{

	vkDestroyPipeline(device_manager->GetLogicalDeviceRef(), graphics_pipeline_subpass0, nullptr);
	vkDestroyPipelineLayout(device_manager->GetLogicalDeviceRef(), pipeline_layout_subpass0, nullptr);

	vkDestroyPipeline(device_manager->GetLogicalDeviceRef(), graphics_pipeline_subpass1, nullptr);
	vkDestroyPipelineLayout(device_manager->GetLogicalDeviceRef(), pipeline_layout_subpass1, nullptr);


}

void Renderer::CleanUpRenderPass()
{
	vkDestroyRenderPass(device_manager->GetLogicalDeviceRef(), render_pass, nullptr);

}

void Renderer::CleanUpImages()
{



	for (auto& var : red_color_attachment) {
		var.CleanUp();
	}


	for (auto& var : depth_attachment) {
		var.CleanUp();
	}


	human_face.CleanUp();



}


void Renderer::CreateGraphicsPiplineLayout()
{



	VkPipelineLayoutCreateInfo pipelineLayoutInfo_subpass0{};
	pipelineLayoutInfo_subpass0.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo_subpass0.setLayoutCount = 1;
	pipelineLayoutInfo_subpass0.pSetLayouts = &descriptor_set_layout_write_subpass0;
	pipelineLayoutInfo_subpass0.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo_subpass0.pPushConstantRanges = nullptr; // Optional


	if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass0, nullptr, &pipeline_layout_subpass0) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}




	VkPipelineLayoutCreateInfo pipelineLayoutInfo_subpass1{};
	pipelineLayoutInfo_subpass1.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo_subpass1.setLayoutCount = 1;
	pipelineLayoutInfo_subpass1.pSetLayouts = &descriptor_set_layout_read_subpass1;
	pipelineLayoutInfo_subpass1.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo_subpass1.pPushConstantRanges = nullptr; // Optional


	if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass1, nullptr, &pipeline_layout_subpass1) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}












}

void Renderer::CreateTextureImages()
{

	human_face.InitTexture(std::string("textures//face.jpg"), device_manager, window, graphics_command_pool);
	human_face.InitTextureView(VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	human_face.InitSampler();




}




void Renderer::CreateAttachmentImages()
{


	red_color_attachment.resize(swapchain_manager->GetSwapImageCount());
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {

		red_color_attachment[i].Init(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, swapchain_manager->GetSwapChainImageExtent(), 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		red_color_attachment[i].TransitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue());


		red_color_attachment[i].InitImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

	}






}
