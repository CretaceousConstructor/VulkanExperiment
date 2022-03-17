#include "Multisubpass.h"

void MultiSubpassesRenderer::PrepareModels()
{
	//========================================================================================================================

	test_model = std::make_unique<GltfModel>(std::string("../../data/models/FlightHelmet/FlightHelmet.gltf"), device_manager, window, window->GetSurface(), graphics_command_pool, transfer_command_buffer);
}

void MultiSubpassesRenderer::CreateDescriptorPool()
{
	// Create the global descriptor pool
	std::array<VkDescriptorPoolSize, 3> poolSizes{};

	poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 6;        // 6        uniform_buffers  uniform_buffers_gs ��������3֡��2*3 = 6

	poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;        //����ģ��ʹ�õĲ�����Ŀ����
	poolSizes[1].descriptorCount = test_model->images.size();

	poolSizes[2].type            = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	poolSizes[2].descriptorCount = 6;        //6          depthcolor ��rcolor ��������3֡��2*3 = 6��rcolor�������������Եģ�û��ʵ����;

	VkDescriptorPoolCreateInfo descriptorPoolCI = {};
	descriptorPoolCI.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCI.poolSizeCount              = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCI.pPoolSizes                 = poolSizes.data();
	// Max. number of descriptor sets that can be allocated from this pool (one per object)

	//read descriptor sets 3 ��,write descriptor sets 3 ��
	//TODO:��������sets��Ŀ
	//TODO:��������sets��Ŀ
	descriptorPoolCI.maxSets = static_cast<uint32_t>(test_model->images.size()) + 6;

	if (vkCreateDescriptorPool(device_manager->GetLogicalDeviceRef(), &descriptorPoolCI, nullptr, &descriptor_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
void MultiSubpassesRenderer::CreateDescriptorSetLayout()
{
	/*
			Attachment write
	*/
	{
	    // Pipeline layout using both descriptor sets (set 0 = matrices, set 1 = material for model)


		//LAYOUT FOR SET 0
		// Descriptor set layout for passing matrices
	    {

	        std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass0;
			VkDescriptorSetLayoutBinding LayoutBindingSubpass0_temp{};

			LayoutBindingSubpass0_temp.binding            = 0;        //uniform buffer
			LayoutBindingSubpass0_temp.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			LayoutBindingSubpass0_temp.descriptorCount    = 1;
			LayoutBindingSubpass0_temp.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; //uniform buffer will be used both in VS and FS stages
			LayoutBindingSubpass0_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass0.push_back(LayoutBindingSubpass0_temp);


			LayoutBindingSubpass0_temp.binding            = 1;        //uniform buffer for Geometry shader
			LayoutBindingSubpass0_temp.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			LayoutBindingSubpass0_temp.descriptorCount    = 1;
			LayoutBindingSubpass0_temp.stageFlags         = VK_SHADER_STAGE_GEOMETRY_BIT;
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



		//LAYOUT FOR SET 1
		// Descriptor set layout for passing material textures
		{

			std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass0;
			VkDescriptorSetLayoutBinding              LayoutBindingSubpass0_temp{};

			//TODO: check the binding number here
			//���ʵ�texture����set = 1��ʹ�ã�set 1�е�binding 0 ��û�б�ʹ�ã����Ե�Ȼ�����֮ǰ��matrix UB������ͻ
			LayoutBindingSubpass0_temp.binding            = 0;        //index  texture mapping
			LayoutBindingSubpass0_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			LayoutBindingSubpass0_temp.descriptorCount    = 1;
			LayoutBindingSubpass0_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBindingSubpass0_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass0.push_back(LayoutBindingSubpass0_temp);

			VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass0{};
			LayoutBindingCISubpass0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			LayoutBindingCISubpass0.bindingCount = (uint32_t) LayoutBindingSubpass0.size();        //the amount of VkDescriptorSetLayoutBinding
			LayoutBindingCISubpass0.pBindings    = LayoutBindingSubpass0.data();
			if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass0, nullptr, &descriptor_set_layout_write_subpass0_materials) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}
}

	/*
		Attachment read
	*/
	//LAYOUT FOR SET 0
	{
		std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass1;

		VkDescriptorSetLayoutBinding LayoutBindingSubpass1_temp{};

		LayoutBindingSubpass1_temp.binding         = 0;        //color for  read
		LayoutBindingSubpass1_temp.descriptorCount = 1;
		LayoutBindingSubpass1_temp.descriptorType  = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		LayoutBindingSubpass1_temp.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
		LayoutBindingSubpass1.push_back(LayoutBindingSubpass1_temp);

		LayoutBindingSubpass1_temp.binding         = 1;        //depth value for read
		LayoutBindingSubpass1_temp.descriptorCount = 1;
		LayoutBindingSubpass1_temp.descriptorType  = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		LayoutBindingSubpass1_temp.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
		LayoutBindingSubpass1.push_back(LayoutBindingSubpass1_temp);

		VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass1{};
		LayoutBindingCISubpass1.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutBindingCISubpass1.bindingCount = (uint32_t) LayoutBindingSubpass1.size();        //the amount of VkDescriptorSetLayoutBinding
		LayoutBindingCISubpass1.pBindings    = LayoutBindingSubpass1.data();

		if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass1, nullptr, &descriptor_set_layout_read_subpass1) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
}

void MultiSubpassesRenderer::CreateDescriptorSets()
{
	descriptor_sets_write_subpass0.resize(swapchain_manager->GetSwapImageCount());
	descriptor_sets_read_subpass1.resize(swapchain_manager->GetSwapImageCount());

	//subpass0 writing
	{

		//set = 0
		//Descriptor sets for vs matrix and gs UB 
		for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
		{

			//ALLOCATE DESCRIPTORS
			VkDescriptorSetAllocateInfo allocInfoWrite{};
			allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfoWrite.descriptorPool     = descriptor_pool;
			allocInfoWrite.descriptorSetCount = 1;
			allocInfoWrite.pSetLayouts        = &descriptor_set_layout_write_subpass0;

			if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &descriptor_sets_write_subpass0[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
			//DESCRIPTORS INFOS
			VkDescriptorBufferInfo buffer_info_write_subpass0{};
			buffer_info_write_subpass0.buffer = uniform_buffers[i].buffer;
			buffer_info_write_subpass0.offset = 0;
			buffer_info_write_subpass0.range  = sizeof(ShaderData);


			VkDescriptorBufferInfo buffer_info_write_subpass0_GS{};
			buffer_info_write_subpass0_GS.buffer = uniform_buffers_GS[i].buffer;
			buffer_info_write_subpass0_GS.offset = 0;
			buffer_info_write_subpass0_GS.range  = sizeof(UniformBufferOjectGS);


			//UPDATE DESCRIPTORS 
			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			VkWriteDescriptorSet              temp_writeDescriptorSet{};
			/*
				Binding 0: VS Object matrices Uniform buffer
			*/
			temp_writeDescriptorSet.sType          = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet         = descriptor_sets_write_subpass0[i];
			temp_writeDescriptorSet.dstBinding     = 0;
			temp_writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//Otherwise, descriptorCount is one of the number of elements in pImageInfo the number of elements in pBufferInfo
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.pBufferInfo     = &buffer_info_write_subpass0;
			temp_writeDescriptorSet.dstArrayElement = 0;

			writeDescriptorSets.push_back(temp_writeDescriptorSet);
			/*
				Binding 1: GS uniform buffer
			*/
			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_write_subpass0[i];
			temp_writeDescriptorSet.dstBinding      = 1;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			temp_writeDescriptorSet.pBufferInfo     = &buffer_info_write_subpass0_GS;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
		}
		// set=1
		// Descriptor sets for materials of models
		for (auto &image : test_model->images)
		{
			//ALLOCATE DESCRIPTORS
			VkDescriptorSetAllocateInfo allocInfoWrite{};
			allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfoWrite.descriptorPool     = descriptor_pool;
			allocInfoWrite.descriptorSetCount = 1;
			allocInfoWrite.pSetLayouts        = &descriptor_set_layout_write_subpass0_materials;

			if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &image.descriptorSet) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
			//DESCRIPTOR INFOS
			VkWriteDescriptorSet temp_writeDescriptorSet{};

			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = image.descriptorSet;
			temp_writeDescriptorSet.dstBinding      = 0;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			auto image_info                         = image.texture.GetDescriptor();
			temp_writeDescriptorSet.pImageInfo      = &image_info;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.dstArrayElement = 0;
			
			//UPDATE DESCRIPTOR SET	
			vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), 1, &temp_writeDescriptorSet, 0, nullptr);
		}
	}

	//subpass1 reading
	//set = 0 
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{

		//ALLOCATE DESCRIPTORS
		VkDescriptorSetAllocateInfo allocInfoRead{};
		allocInfoRead.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfoRead.descriptorPool     = descriptor_pool;
		allocInfoRead.descriptorSetCount = 1;
		allocInfoRead.pSetLayouts        = &descriptor_set_layout_read_subpass1;

		if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoRead, &descriptor_sets_read_subpass1[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}



		//DESCRIPTOR INFOS
		std::array<VkDescriptorImageInfo, 2> imageInfoForSubPass1{};
		imageInfoForSubPass1[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForSubPass1[0].imageView   = all_color_attachment[i].GetImageView();
		imageInfoForSubPass1[0].sampler     = VK_NULL_HANDLE;

		imageInfoForSubPass1[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForSubPass1[1].imageView   = depth_attachment[i].GetImageView();
		imageInfoForSubPass1[1].sampler     = VK_NULL_HANDLE;

		//UPDATE DESCRIPTOR SET	
		std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};

		/*
			Binding 0: color
		*/
		writeDescriptorSets[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet          = descriptor_sets_read_subpass1[i];
		writeDescriptorSets[0].descriptorType  = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[0].dstBinding      = 0;
		writeDescriptorSets[0].pImageInfo      = &imageInfoForSubPass1[0];
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].pBufferInfo     = VK_NULL_HANDLE;

		/*
			Binding 1: depthValue
		*/
		writeDescriptorSets[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet          = descriptor_sets_read_subpass1[i];
		writeDescriptorSets[1].dstBinding      = 1;
		writeDescriptorSets[1].descriptorType  = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[1].pImageInfo      = &imageInfoForSubPass1[1];
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].pBufferInfo     = VK_NULL_HANDLE;

		vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void MultiSubpassesRenderer::CreateUniformBuffer()
{
	//GPU SIDE
	VkDeviceSize bufferSize = sizeof(ShaderData);
	uniform_buffers.resize(swapchain_manager->GetSwapImageCount());
	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers[i].buffer, uniform_buffers[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());
		//exclusive mode��Ϊuniform bufferֻ�ᱻgraphics queueʹ��
	}


	VkDeviceSize bufferSizeGS = sizeof(UniformBufferOjectGS);
	uniform_buffers_GS.resize(swapchain_manager->GetSwapImageCount());
	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		device_manager->CreateBuffer(bufferSizeGS, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_GS[i].buffer, uniform_buffers_GS[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());
		//exclusive mode��Ϊuniform bufferֻ�ᱻgraphics queueʹ��
	}

	
	
	
	////CPU SIDE
	//ubo.model = viking_room_model->GetTransform().GetLocalToWorldMatrix();
	//ubo.view = m_pCamera->GetView();
	//ubo.proj = m_pCamera->GetProj();

	//ubo0.model = viking_room_model->GetTransform().GetLocalToWorldMatrix();
	//ubo0.view = m_pCamera->GetView();
}

void MultiSubpassesRenderer::CreateDepthImages()
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

void MultiSubpassesRenderer::CreatePiplineSubpass0()
{
	//										 subpass0
	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	/******************************************************************************************************/
	ShaderManager vertex_shader_subpass0(std::string("../../data/shaders/multisubpasses/multisubpass_vertex_shader_subpass0.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_subpass0(std::string("../../data/shaders/multisubpasses/multisubpass_fragment_shader_subpass0.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());

	ShaderManager vertex_shader_subpass0_normal(std::string("../../data/shaders/multisubpasses/multisubpass_vertex_shader_subpass0_normal.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager geometry_shader_subpass0_normal(std::string("../../data/shaders/multisubpasses/multisubpass_geometry_shader_subpass0_normal.spv"), std::string("main"), VK_SHADER_STAGE_GEOMETRY_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_subpass0_normal(std::string("../../data/shaders/multisubpasses/multisubpass_fragment_shader_subpass0_normal.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info =
	    {vertex_shader_subpass0_normal.GetVkPipelineShaderStageCreateInfo(), geometry_shader_subpass0_normal.GetVkPipelineShaderStageCreateInfo(), fragment_shader_subpass0_normal.GetVkPipelineShaderStageCreateInfo()};

	//TODO:��Ҫ�����abstraction
	VkVertexInputBindingDescription bindingDescription0{};

	bindingDescription0.binding   = 0;
	bindingDescription0.stride    = sizeof(GltfModel::Vertex);
	bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	//VkVertexInputBindingDescription bindingDescription1{};
	//bindingDescription1.binding = 1;
	//bindingDescription1.stride = sizeof(InsatnceTranformation);
	//bindingDescription1.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	//std::vector<VkVertexInputBindingDescription>  VIBDS = { bindingDescription0 ,bindingDescription1 };
	
	
	std::vector<VkVertexInputBindingDescription> VIBDS = {bindingDescription0};

	auto attributeDescriptions = GltfModel::Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
	vertex_input_info.pVertexBindingDescriptions      = VIBDS.data();
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();    

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

	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;

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

	//
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 0;
	dynamicState.pDynamicStates    = VK_NULL_HANDLE;

	VkPipelineDepthStencilStateCreateInfo depthStencil_supass0{};
	depthStencil_supass0.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil_supass0.depthTestEnable       = VK_TRUE;
	depthStencil_supass0.depthWriteEnable      = VK_TRUE;
	depthStencil_supass0.depthCompareOp        = VK_COMPARE_OP_LESS;
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
	//this pipline is for debugging purpose,to show normals of a model.
	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline_subpass0_CI, nullptr, &graphics_pipeline_subpass0_normal) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	shader_stages_create_info       = {vertex_shader_subpass0.GetVkPipelineShaderStageCreateInfo(), fragment_shader_subpass0.GetVkPipelineShaderStageCreateInfo()};
	pipeline_subpass0_CI.stageCount = (uint32_t) shader_stages_create_info.size();
	pipeline_subpass0_CI.pStages    = shader_stages_create_info.data();




	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline_subpass0_CI, nullptr, &graphics_pipeline_subpass0) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}


}

void MultiSubpassesRenderer::CreatePiplineSubpass1()
{
	/******************************************************************************************************/
	ShaderManager vertex_shader_subpass1(std::string("..//..//data//shaders//multisubpasses//multisubpass_vertex_shader_subpass1.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_subpass1(std::string("..//..//data//shaders//multisubpasses//multisubpass_fragment_shader_subpass1.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info = {vertex_shader_subpass1.GetVkPipelineShaderStageCreateInfo(), fragment_shader_subpass1.GetVkPipelineShaderStageCreateInfo()};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable        = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth               = 1.f;
	rasterizer.cullMode                = VK_CULL_MODE_NONE;
	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkPipelineDepthStencilStateCreateInfo depthStencil_supass1{};
	depthStencil_supass1.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil_supass1.depthTestEnable       = VK_TRUE;
	depthStencil_supass1.depthWriteEnable      = VK_FALSE;
	depthStencil_supass1.depthCompareOp        = VK_COMPARE_OP_LESS;
	depthStencil_supass1.depthBoundsTestEnable = VK_FALSE;
	depthStencil_supass1.minDepthBounds        = 0.0f;        // Optional
	depthStencil_supass1.maxDepthBounds        = 1.0f;        // Optional
	depthStencil_supass1.stencilTestEnable     = VK_FALSE;
	//depthStencil_supass1.front = {}; // Optional
	//depthStencil_supass1.back = {}; // Optional

	VkPipelineVertexInputStateCreateInfo empty_input_stateCI{};
	empty_input_stateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

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

	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	VkViewport       viewport{};
	viewport.x        = 0.0f;
	viewport.y        = (float) extend_of_swap_image.height;
	viewport.width    = (float) extend_of_swap_image.width;
	viewport.height   = -(float) extend_of_swap_image.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

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

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable   = VK_FALSE;
	multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading      = 1.0f;            // Optional
	multisampling.pSampleMask           = nullptr;         // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE;        // Optional
	multisampling.alphaToOneEnable      = VK_FALSE;        // Optional

	VkGraphicsPipelineCreateInfo pipeline_subpass1_CI{};

	pipeline_subpass1_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_subpass1_CI.stageCount = (uint32_t) shader_stages_create_info.size();
	pipeline_subpass1_CI.pStages    = shader_stages_create_info.data();

	pipeline_subpass1_CI.pVertexInputState   = &empty_input_stateCI;
	pipeline_subpass1_CI.pInputAssemblyState = &inputAssembly;
	pipeline_subpass1_CI.pViewportState      = &viewportState;
	pipeline_subpass1_CI.pRasterizationState = &rasterizer;
	pipeline_subpass1_CI.pMultisampleState   = &multisampling;
	pipeline_subpass1_CI.pDepthStencilState  = &depthStencil_supass1;

	pipeline_subpass1_CI.pColorBlendState = &colorBlending;
	pipeline_subpass1_CI.pDynamicState    = nullptr;        // Optional

	pipeline_subpass1_CI.layout = pipeline_layout_subpass1;

	pipeline_subpass1_CI.renderPass = render_pass;
	pipeline_subpass1_CI.subpass    = 1;        // index

	pipeline_subpass1_CI.basePipelineIndex  = -1;
	pipeline_subpass1_CI.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline_subpass1_CI, nullptr, &graphics_pipeline_subpass1) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

void MultiSubpassesRenderer::CreateRenderPass()
{
	//swapchain color index 0
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format         = swapchain_manager->GetSwapChainImageFormat();
	colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//R color index 1
	VkAttachmentDescription AllchannelcolorAttachment{};
	AllchannelcolorAttachment.format = swapchain_manager->GetSwapChainImageFormat();        //�����format�Ƿ�������һ�£�

	AllchannelcolorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	AllchannelcolorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	AllchannelcolorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	AllchannelcolorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	AllchannelcolorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	AllchannelcolorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	AllchannelcolorAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//Depth index 2
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format         = swapchain_manager->FindDepthFormat(*device_manager);
	depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//The index of the attachment ref is directly referenced from the fragment shader with the
	//layout(location = 0) out vec4 outColor statement!

	//subpass0�õ�
	VkAttachmentReference RAttachmentRef{};
	RAttachmentRef.attachment = 1;
	RAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRefForWrite{};
	depthAttachmentRefForWrite.attachment = 2;
	depthAttachmentRefForWrite.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



	//subpass1�õ�
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference RAttachmentRefForRead{};
	RAttachmentRefForRead.attachment = 1;
	RAttachmentRefForRead.layout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRefForRead{};
	depthAttachmentRefForRead.attachment = 2;
	depthAttachmentRefForRead.layout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


	//======================================================================================================
	std::vector<VkAttachmentReference> WriteColorAttachmentRefsForSubpass0 = {RAttachmentRef};
	VkSubpassDescription               subpass0{};
	subpass0.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass0.colorAttachmentCount    = static_cast<uint32_t>(WriteColorAttachmentRefsForSubpass0.size());
	subpass0.pColorAttachments       = WriteColorAttachmentRefsForSubpass0.data();
	subpass0.pDepthStencilAttachment = &depthAttachmentRefForWrite;

	//======================================================================================================
	std::vector<VkAttachmentReference> outPutColorAttachmentRefsForSubpass1 = {colorAttachmentRef};
	std::vector<VkAttachmentReference> inPutAttachmentRefsForSubpass1       = {RAttachmentRefForRead, depthAttachmentRefForRead};

	VkSubpassDescription subpass1{};
	subpass1.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass1.colorAttachmentCount    = static_cast<uint32_t>(outPutColorAttachmentRefsForSubpass1.size());
	subpass1.pColorAttachments       = outPutColorAttachmentRefsForSubpass1.data();
	subpass1.inputAttachmentCount    = static_cast<uint32_t>(inPutAttachmentRefsForSubpass1.size());
	subpass1.pInputAttachments       = inPutAttachmentRefsForSubpass1.data();
	subpass1.pDepthStencilAttachment = VK_NULL_HANDLE;

	//-------------------------------------------------------------------------------------
	std::array<VkSubpassDescription, 2> subpasses = {subpass0, subpass1};

	std::vector<VkAttachmentDescription> attachments = {colorAttachment, AllchannelcolorAttachment, depthAttachment};








	std::array<VkSubpassDependency, 2> dependencies{};

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;

	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	dependencies[0].srcAccessMask = 0;

	dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// This dependency transitions the input attachment from color attachment to shader read only layout 
	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = 1;

	dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//dependencies[2].srcSubpass    = 1;        // Last subpass 
	//dependencies[2].dstSubpass    = VK_SUBPASS_EXTERNAL;
	//dependencies[2].srcStageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	//dependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	//dependencies[2].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	//dependencies[2].dstAccessMask   = 0;
	//dependencies[2].dependencyFlags = 0;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies   = dependencies.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments    = attachments.data();
	renderPassInfo.subpassCount    = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses      = subpasses.data();

	if (vkCreateRenderPass(device_manager->GetLogicalDeviceRef(), &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void MultiSubpassesRenderer::CreateGraphicsPiplineLayout()
{
	{
		std::vector<VkDescriptorSetLayout> setLayouts = {descriptor_set_layout_write_subpass0, descriptor_set_layout_write_subpass0_materials};
		VkPipelineLayoutCreateInfo         pipelineLayoutInfo_subpass0{};
		pipelineLayoutInfo_subpass0.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo_subpass0.setLayoutCount = setLayouts.size();
		pipelineLayoutInfo_subpass0.pSetLayouts    = setLayouts.data();

		//TODO: testing multiple push constants and how to access it
		// We will use push constants to push the local matrices of a primitive to the vertex shader
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.size       = sizeof(glm::mat4);
		pushConstantRange.offset     = 0;

		pipelineLayoutInfo_subpass0.pushConstantRangeCount = 1;                         // Optional
		pipelineLayoutInfo_subpass0.pPushConstantRanges    = &pushConstantRange;        // Optional

		if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass0, nullptr, &pipeline_layout_subpass0) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo_subpass1{};
		pipelineLayoutInfo_subpass1.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo_subpass1.setLayoutCount         = 1;
		pipelineLayoutInfo_subpass1.pSetLayouts            = &descriptor_set_layout_read_subpass1;
		pipelineLayoutInfo_subpass1.pushConstantRangeCount = 0;              // Optional
		pipelineLayoutInfo_subpass1.pPushConstantRanges    = nullptr;        // Optional

		if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass1, nullptr, &pipeline_layout_subpass1) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
}

void MultiSubpassesRenderer::CreateGraphicsPipline()
{
	system("..\\..\\data\\shaderbat\\MultiSubpassShaderCompile.bat");

	CreatePiplineSubpass0();
	CreatePiplineSubpass1();
}

void MultiSubpassesRenderer::CreateFramebuffers()
{
	frame_buffers.resize(swapchain_manager->GetSwapImageCount());




	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		std::vector<VkImageView> attachments = {
		    swapchain_manager->GetSwapImageViews()[i],
		    all_color_attachment[i].GetImageView(),
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

void MultiSubpassesRenderer::InitCommandBuffers()
{
	VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), transfor_command_pool, transfer_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	graphics_command_buffers.resize(swapchain_manager->GetSwapImageCount());

	for (int i = 0; i < graphics_command_buffers.size(); i++)
	{
		VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), graphics_command_pool, graphics_command_buffers[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}


}

void MultiSubpassesRenderer::CommandBufferRecording()
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
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		//renderPassInfo.renderPass = render_pass;
		renderPassInfo.renderPass = render_pass;

		renderPassInfo.framebuffer = frame_buffers[i];

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = VkExtent2D{swapchain_manager->GetSwapChainImageExtent().width, swapchain_manager->GetSwapChainImageExtent().height};

		std::array<VkClearValue, 3> clearValues{};
		clearValues[0].color        = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].color        = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[2].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues    = clearValues.data();

		vkCmdBeginRenderPass(graphics_command_buffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);        //

		///*
		//		First sub pass
		//		Fills the attachments
		//*/

		vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_subpass0, 0, 1, &descriptor_sets_write_subpass0[i], 0, NULL);
		vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_subpass0);
		test_model->Draw(graphics_command_buffers[i], pipeline_layout_subpass0);

		vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_subpass0_normal);
		test_model->Draw(graphics_command_buffers[i], pipeline_layout_subpass0);








		//	Second sub pass
		//	Render a full screen quad, reading from the previously written attachments via input attachments
		//*/

		vkCmdNextSubpass(graphics_command_buffers[i], VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_subpass1);
		vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_subpass1, 0, 1, &descriptor_sets_read_subpass1[i], 0, NULL);

		vkCmdDraw(graphics_command_buffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(graphics_command_buffers[i]);

		if (vkEndCommandBuffer(graphics_command_buffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void MultiSubpassesRenderer::InitSynObjects()
{
	image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	frame_fences.resize(MAX_FRAMES_IN_FLIGHT);
	images_fences.resize(swapchain_manager->GetSwapImageCount(), VK_NULL_HANDLE);        ///???

	//����ͨ��fence��ѯvkQueueSubmit�Ķ����Ƿ����   vkGetFenceStatus�������Ĳ�ѯ
	//											   vkWaitForFences������ѯ��ֱ����������һ�����������е�fence������signaled״̬�����߳�ʱ��ʱ�������ɲ������������Ż᷵�ء�

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

void MultiSubpassesRenderer::CreateTextureImages()
{
	//UNORM
	//swapchain_manager->GetSwapChainImageFormat()�õ�����
	//texture����ʹ��
	//VK_FORMAT_B8G8R8A8_SRGB
	//VK_FORMAT_R8G8B8A8_UNORM
	//
	

	//human_face.InitTexture(std::string("..//..//data//textures//face.jpg"), device_manager, window, graphics_command_pool, swapchain_manager->GetSwapChainImageFormat());
	//human_face.InitTextureView(swapchain_manager->GetSwapChainImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
	//human_face.InitSampler();

	//viking_room.InitTexture(std::string("..//..//data//textures//viking_room.png"), device_manager, window, graphics_command_pool, swapchain_manager->GetSwapChainImageFormat());
	//viking_room.InitTextureView(swapchain_manager->GetSwapChainImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
	//viking_room.InitSampler();

	human_face.InitTexture(std::string("..//..//data//textures//face.jpg"), device_manager, window, graphics_command_pool, VK_FORMAT_R8G8B8A8_SRGB);
	human_face.InitTextureView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	human_face.InitSampler();

	viking_room.InitTexture(std::string("..//..//data//textures//viking_room.png"), device_manager, window, graphics_command_pool, VK_FORMAT_R8G8B8A8_SRGB);
	viking_room.InitTextureView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	viking_room.InitSampler();
}

void MultiSubpassesRenderer::SetUpUserInput()
{
	std::vector<int> tracked_keys = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_Z, GLFW_KEY_C, GLFW_KEY_UP, GLFW_KEY_DOWN};
	keyboard                      = std::make_unique<KeyBoardInputManager>(tracked_keys);
	keyboard->SetupKeyInputs(window->GetWindowPtr());

	mouse = std::make_unique<MouseInputManager>(swapchain_manager->GetSwapChainImageExtent());
	mouse->SetupMouseInputs(window->GetWindowPtr());
}

void MultiSubpassesRenderer::CreateAttachmentImages()
{
	//swapchain_manager->GetSwapChainImageFormat()

	//VK_FORMAT_R8G8B8A8_UNORM



	all_color_attachment.resize(swapchain_manager->GetSwapImageCount());

	VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager->FindQueueFamilies(device_manager->GetPhysicalDeviceRef(), window->GetSurface());
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		all_color_attachment[i].Init(VK_IMAGE_TYPE_2D, swapchain_manager->GetSwapChainImageFormat(), swapchain_manager->GetSwapChainImageExtent(), 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);





		all_color_attachment[i].TransitionImageLayout(swapchain_manager->GetSwapChainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue(), queue_family_index);

		all_color_attachment[i].InitImageView(swapchain_manager->GetSwapChainImageFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void MultiSubpassesRenderer::DrawFrame()
{
	static size_t currentFrame = 0;

	vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &frame_fences[currentFrame], VK_TRUE, UINT64_MAX);        //vkWaitForFences����ʱ����CPU���ȴ�fence��signal�� �� unsignaled״̬ ��� signaled״̬ ����Ӧ���Ƿ�ֹ���Լ�(currentFrame)��ͻ��To wait for one or more fences to enter the signaled state on the host,

	uint32_t imageIndex;

	VkResult result = vkAcquireNextImageKHR(device_manager->GetLogicalDeviceRef(), swapchain_manager->GetSwapChain(), UINT64_MAX, image_available_semaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);        //As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.�õ���һ������ʹ�õ�image��index���������image���ܻ�û���꣬�����õ�imageIndex��Ӧ��image���п�������챻ĳһ֡ʹ����ϵ���һ������vulkanʵ�־������

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

	//images_fences��СΪ3����������ע�����Ĵ�С����MAX_FRAMES_IN_FLIGHT
	if (images_fences[imageIndex] != VK_NULL_HANDLE)
	{        //images_fences[imageIndex] ���� nullptr��˵��ĳһ֡��GPU��Դ���ڱ���imageIndexΪ�±��imageʹ�ã���ô���Ǿ�Ҫ�ȴ���
		vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &images_fences[imageIndex], VK_TRUE, UINT64_MAX);
	}

	images_fences[imageIndex] = frame_fences[currentFrame];        //�ȴ���Ϻ��images�������ǵ�currentFrame��ռ��(��ʾcurrentFrame��һ֡��GPU��Դ���ڱ�indexΪimageIndex��imageռ��)��Ŀǰframe_fences[currentFrame]����signled��״̬��
	//	 frame_fences[currentFrame]״̬�ı��images_fences[imageIndex]״̬Ҳ��ı�

	VkSubmitInfo submitInfo{};
	submitInfo.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore          waitSemaphores[] = {image_available_semaphores[currentFrame]};        //�����ǵȴ�imageIndex��Ӧ��image����ռ�õ�GPU��Դ�ͷų�������������Ϊimage����Ҫpresentation�����п���presentation engine��û����ɣ�������Ҫ�ȴ����semaphore
	VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount         = 1;
	submitInfo.pWaitSemaphores            = waitSemaphores;
	submitInfo.pWaitDstStageMask          = waitStages;
	submitInfo.commandBufferCount         = 1;
	submitInfo.pCommandBuffers            = &graphics_command_buffers[imageIndex];
	VkSemaphore signalSemaphores[]        = {render_finished_semaphores[currentFrame]};        //graphics_command_buffersִ�����Ժ��signal������presentation engine֪����Ⱦ��ɿ���չʾ�ˡ�
	submitInfo.signalSemaphoreCount       = 1;
	submitInfo.pSignalSemaphores          = signalSemaphores;

	//��Ϊ������images_fences[imageIndex] = frame_fences[currentFrame]; ������ʱ��images_fences[imageIndex]�� frame_fences[currentFrame];Ӧ��ͬʱ������unsignaled״̬
	vkResetFences(device_manager->GetLogicalDeviceRef(), 1, &frame_fences[currentFrame]);        //To set the state of fences to unsignaled from the host

	if (vkQueueSubmit(device_manager->GetGraphicsQueue(), 1, &submitInfo, frame_fences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//fence is an optional handle to a fence to be signaled once all submitted command buffers have completed execution.If fence is not VK_NULL_HANDLE, it defines a fence signal operation.

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

void MultiSubpassesRenderer::UpdateUniformBuffer(uint32_t currentImage)
{
	ubo.proj   = m_pCamera->GetProj();
	ubo.view   = m_pCamera->GetView();
	ubo.eyePos = m_pCamera->GetPosition();


	ubo_gs.projection = m_pCamera->GetProj();
	ubo_gs.view       = m_pCamera->GetView();

	void *data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers[currentImage].memory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ShaderData));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers[currentImage].memory);

	void *data_gs;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_GS[currentImage].memory, 0, sizeof(ubo_gs), 0, &data_gs);
	memcpy(data_gs, &ubo_gs, sizeof(UniformBufferOjectGS));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_GS[currentImage].memory);




}

void MultiSubpassesRenderer::UpdateCamera(float dt)
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

void MultiSubpassesRenderer::CreateCamera()
{
	m_pCamera = std::make_unique<FirstPersonCamera>();
	m_pCamera->SetFrustum(glm::radians(60.f), swapchain_manager->GetSwapChainImageExtent().width / (float) swapchain_manager->GetSwapChainImageExtent().height, 0.1f, 256.f);
	m_pCamera->LookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0., 1., 0.));

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

void MultiSubpassesRenderer::CleanUpModels()
{
}

void MultiSubpassesRenderer::CleanUpDescriptorSetLayoutAndDescriptorPool()
{
	vkDestroyDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), descriptor_set_layout_write_subpass0, nullptr);
	vkDestroyDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), descriptor_set_layout_read_subpass1, nullptr);

	vkDestroyDescriptorPool(device_manager->GetLogicalDeviceRef(), descriptor_pool, nullptr);
}

void MultiSubpassesRenderer::CleanUpCommandBuffersAndCommandPool()
{
	vkFreeCommandBuffers(device_manager->GetLogicalDeviceRef(), graphics_command_pool, static_cast<uint32_t>(graphics_command_buffers.size()), graphics_command_buffers.data());
	vkFreeCommandBuffers(device_manager->GetLogicalDeviceRef(), transfor_command_pool, 1, &transfer_command_buffer);

	vkDestroyCommandPool(device_manager->GetLogicalDeviceRef(), graphics_command_pool, nullptr);
	vkDestroyCommandPool(device_manager->GetLogicalDeviceRef(), transfor_command_pool, nullptr);
}

void MultiSubpassesRenderer::CleanUpSyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device_manager->GetLogicalDeviceRef(), render_finished_semaphores[i], nullptr);
		vkDestroySemaphore(device_manager->GetLogicalDeviceRef(), image_available_semaphores[i], nullptr);
		vkDestroyFence(device_manager->GetLogicalDeviceRef(), frame_fences[i], nullptr);
	}
}

void MultiSubpassesRenderer::CleanupFrameBuffers()
{
	for (auto framebuffer : frame_buffers)
	{
		vkDestroyFramebuffer(device_manager->GetLogicalDeviceRef(), framebuffer, nullptr);
	}
}

void MultiSubpassesRenderer::CleanUpPiplineAndPiplineLayout()
{
	vkDestroyPipeline(device_manager->GetLogicalDeviceRef(), graphics_pipeline_subpass0, nullptr);
	vkDestroyPipelineLayout(device_manager->GetLogicalDeviceRef(), pipeline_layout_subpass0, nullptr);

	vkDestroyPipeline(device_manager->GetLogicalDeviceRef(), graphics_pipeline_subpass1, nullptr);
	vkDestroyPipelineLayout(device_manager->GetLogicalDeviceRef(), pipeline_layout_subpass1, nullptr);
}

void MultiSubpassesRenderer::CleanUpRenderPass()
{
	vkDestroyRenderPass(device_manager->GetLogicalDeviceRef(), render_pass, nullptr);
}

void MultiSubpassesRenderer::CleanUpImages()
{
	for (auto &var : all_color_attachment)
	{
		var.CleanUp();
	}

	for (auto &var : depth_attachment)
	{
		var.CleanUp();
	}

	human_face.CleanUp();
}

void MultiSubpassesRenderer::CleanUpUniformBuffers()
{
}

namespace std
{
template <>
struct hash<MultiSubpassesRenderer::Vertex>
{
	size_t operator()(MultiSubpassesRenderer::Vertex const &vertex) const
	{
		return ((hash<glm::vec3>()(vertex.pos) ^
		         (hash<glm::vec3>()(vertex.color) << 1)) >>
		        1) ^
		       (hash<glm::vec2>()(vertex.texCoord) << 1);
	}
};
}        // namespace std