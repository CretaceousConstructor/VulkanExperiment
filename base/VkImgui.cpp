#include "VkImgui.h"

void VkImgui::GuiData::updateFrameGraph(float frameTime)
{
	lastFrameTime = frameTime * 1000.0f;
	std::rotate(frameTimes.begin(), frameTimes.begin() + 1, frameTimes.end());
	frameTimes.back() = lastFrameTime;
	if (lastFrameTime < frameTimeMin)
	{
		frameTimeMin = lastFrameTime;
	}
	if (lastFrameTime > frameTimeMax)
	{
		frameTimeMax = lastFrameTime;
	}
}

VkImgui::VkImgui(VkDeviceManager *_device_manager, VkWindows *_windows, VkCommandManager *_command_manager) :
    device(_device_manager),
    windows(_windows),
    command_manager(_command_manager)
{
	ImGui::CreateContext();
	// Set a color scheme
	ImGuiStyle &style                    = ImGui::GetStyle();
	style.Colors[ImGuiCol_TitleBg]       = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
	style.Colors[ImGuiCol_MenuBarBg]     = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_Header]        = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_CheckMark]     = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
}

VkImgui::~VkImgui()
{
	ImGui::DestroyContext();
	// Release all Vulkan resources required for rendering imGui
	for (FrameObjects &frame : frameObjects)
	{
		frame.indix.CleanUp(device);
		frame.vertix.CleanUp(device);
	}

	//vkDestroyImage(device->GetLogicalDeviceRef(), fontImage, nullptr);
	//vkDestroyImageView(device->GetLogicalDeviceRef(), fontView, nullptr);
	//vkFreeMemory(device->GetLogicalDeviceRef(), fontMemory, nullptr);
	//vkDestroySampler(device->GetLogicalDeviceRef(), sampler, nullptr);
	font_texture.CleanUp();

	vkDestroyPipelineCache(device->GetLogicalDeviceRef(), pipelineCache, nullptr);
	vkDestroyPipeline(device->GetLogicalDeviceRef(), pipeline, nullptr);
	vkDestroyPipelineLayout(device->GetLogicalDeviceRef(), pipelineLayout, nullptr);
	vkDestroyDescriptorPool(device->GetLogicalDeviceRef(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device->GetLogicalDeviceRef(), descriptorSetLayout, nullptr);
}

void VkImgui::setFrameCount(uint32_t frameCount)
{
	this->frameCount = frameCount;
}

void VkImgui::setVertexShader(VkPipelineShaderStageCreateInfo shader)
{
	vertexShader = shader;
}

void VkImgui::setFragmentShader(VkPipelineShaderStageCreateInfo shader)
{
	fragmentShader = shader;
}

void VkImgui::createOverlayResources(VkRenderPass renderPass)
{
	// Create and upload the font texture provided by ImGui
	// Get the image data for the font atlas that we'll upload to the GPU
	InitFontTexture();
	// Create a descriptor pool
	// To keep descriptor setup simple, we duplicate the combined image sampler for the font atlas texture per frame, so we need frameCount descriptors for that type in the pool
	InitDescriptorPool();
	// Create the descriptor set layout
	InitDescriptorSetLayout();
	//// Create the descriptor sets
	InitDescriptorSets();

	//// Create a pipeline cache
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	if (vkCreatePipelineCache(device->GetLogicalDeviceRef(), &pipelineCacheCreateInfo, nullptr, &pipelineCache) != VK_SUCCESS)
	{
		std::runtime_error("failed to create pipeline cache!");
	}

	// Create a pipeline layout
	// We use push constants to pass some UI related parameters (scale and translation)

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset     = 0;
	pushConstantRange.size       = sizeof(PushConstBlock);

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts    = &descriptorSetLayout;

	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges    = &pushConstantRange;

	if (vkCreatePipelineLayout(device->GetLogicalDeviceRef(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		std::runtime_error("failed to create pipeline layout!");
	}

	// Create a pipeline
	// Enable blending
	VkPipelineColorBlendAttachmentState blendAttachmentState{};
	blendAttachmentState.blendEnable         = VK_TRUE;
	blendAttachmentState.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachmentState.colorBlendOp        = VK_BLEND_OP_ADD;
	blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendState{};
	colorBlendState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments    = &blendAttachmentState;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
	inputAssemblyState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyState.flags                  = 0;
	inputAssemblyState.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizationState{};
	rasterizationState.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationState.polygonMode      = VK_POLYGON_MODE_FILL;
	rasterizationState.cullMode         = VK_CULL_MODE_NONE;
	rasterizationState.frontFace        = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.flags            = 0;
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.lineWidth        = 1.0f;

	VkPipelineDepthStencilStateCreateInfo depthStencilState{};
	depthStencilState.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable  = VK_FALSE;
	depthStencilState.depthWriteEnable = VK_FALSE;
	depthStencilState.depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilState.back.compareOp   = VK_COMPARE_OP_ALWAYS;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount  = 1;
	viewportState.flags         = 0;

	VkPipelineMultisampleStateCreateInfo multisampleState{};
	multisampleState.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.flags                = 0;

	std::vector<VkDynamicState>      dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates    = dynamicStateEnables.data();
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamicState.flags             = 0;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {vertexShader, fragmentShader};
	VkGraphicsPipelineCreateInfo                 pipelineCreateInfo{};
	pipelineCreateInfo.sType              = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout             = pipelineLayout;
	pipelineCreateInfo.renderPass         = renderPass;
	pipelineCreateInfo.flags              = 0;
	pipelineCreateInfo.basePipelineIndex  = -1;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.pRasterizationState = &rasterizationState;
	pipelineCreateInfo.pColorBlendState    = &colorBlendState;
	pipelineCreateInfo.pMultisampleState   = &multisampleState;
	pipelineCreateInfo.pViewportState      = &viewportState;
	pipelineCreateInfo.pDepthStencilState  = &depthStencilState;
	pipelineCreateInfo.pDynamicState       = &dynamicState;
	pipelineCreateInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
	pipelineCreateInfo.pStages             = shaderStages.data();

	// Vertex bindings and attributes based on ImGui vertex definition
	VkVertexInputBindingDescription vInputBindDescription{};
	vInputBindDescription.binding   = 0;
	vInputBindDescription.stride    = sizeof(ImDrawVert);
	vInputBindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vInputAttribDescription_0{};
	vInputAttribDescription_0.location = 0;
	vInputAttribDescription_0.binding  = 0;
	vInputAttribDescription_0.format   = VK_FORMAT_R32G32_SFLOAT;
	vInputAttribDescription_0.offset   = offsetof(ImDrawVert, pos);

	VkVertexInputAttributeDescription vInputAttribDescription_1{};
	vInputAttribDescription_1.location = 1;
	vInputAttribDescription_1.binding  = 0;
	vInputAttribDescription_1.format   = VK_FORMAT_R32G32_SFLOAT;
	vInputAttribDescription_1.offset   = offsetof(ImDrawVert, uv);

	VkVertexInputAttributeDescription vInputAttribDescription_2{};
	vInputAttribDescription_2.location = 2;
	vInputAttribDescription_2.binding  = 0;
	vInputAttribDescription_2.format   = VK_FORMAT_R8G8B8A8_UNORM;
	vInputAttribDescription_2.offset   = offsetof(ImDrawVert, col);

	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
	    vInputAttribDescription_0,        // Location 0: Position
	    vInputAttribDescription_1,        // Location 1: UV
	    vInputAttribDescription_2,        // Location 2: Color
	};

	VkPipelineVertexInputStateCreateInfo vertexInputState{};
	vertexInputState.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.vertexBindingDescriptionCount   = 1;
	vertexInputState.pVertexBindingDescriptions      = &vInputBindDescription;
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	vertexInputState.pVertexAttributeDescriptions    = vertexInputAttributes.data();
	pipelineCreateInfo.pVertexInputState             = &vertexInputState;

	if (vkCreateGraphicsPipelines(device->GetLogicalDeviceRef(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		std::runtime_error("failed to create graphics pipeline!");
	}
}

void VkImgui::InitFontTexture()
{
	unsigned char *fontData;
	int            texWidth, texHeight;
	ImGuiIO &      io = ImGui::GetIO();
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
	VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

	font_texture.InitTexture(fontData, uploadSize, texWidth, texHeight, device, windows, command_manager->transfer_command_pool, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	font_texture.InitTextureView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.magFilter     = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter     = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	font_texture.InitSampler(samplerCreateInfo);
}

void VkImgui::InitDescriptorPool()
{
	VkDescriptorPoolSize descriptorPoolSize{};
	descriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize.descriptorCount = frameCount;

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes    = &descriptorPoolSize;
	descriptorPoolInfo.maxSets       = frameCount;

	if (vkCreateDescriptorPool(device->GetLogicalDeviceRef(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		std::runtime_error("failed to create descriptor pool!");
	}
}

void VkImgui::InitDescriptorSets()
{
	frameObjects.resize(frameCount);
	for (FrameObjects &frame : frameObjects)
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool     = descriptorPool;
		descriptorSetAllocateInfo.pSetLayouts        = &descriptorSetLayout;
		descriptorSetAllocateInfo.descriptorSetCount = 1;

		if (vkAllocateDescriptorSets(device->GetLogicalDeviceRef(), &descriptorSetAllocateInfo, &frame.descriptorSet) != VK_SUCCESS)
		{
			std::runtime_error("failed to create descriptor set!");
		}

		VkDescriptorImageInfo descriptorImageInfo{font_texture.GetDescriptor()};

		//VkDescriptorImageInfo fontDescriptor     = vks::initializers::descriptorImageInfo(sampler, fontView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet          = frame.descriptorSet;
		writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.dstBinding      = 0;
		writeDescriptorSet.pImageInfo      = &descriptorImageInfo;
		writeDescriptorSet.descriptorCount = 1;
		vkUpdateDescriptorSets(device->GetLogicalDeviceRef(), 1, &writeDescriptorSet, 0, nullptr);
	}
}

void VkImgui::newFrame()
{
	// Start a new ImGui frame
	ImGui::NewFrame();
	// Add a window with some performance data
	ImGui::SetNextWindowPos(ImVec2(100, 40), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(325, 180), ImGuiCond_FirstUseEver);
	ImGui::Begin("Performance");
	ImGui::TextUnformatted("Vulkan Example - Dear ImGui");
	ImGui::TextUnformatted("Nvdia GTX 1650");
	ImGui::Text("%.2f ms/frame", guiData.lastFrameTime);
	ImGui::PlotLines("Frame Times", &guiData.frameTimes[0], 50, 0, "", guiData.frameTimeMin, guiData.frameTimeMax, ImVec2(0, 80));
	ImGui::End();
	// Add another window with some settings
	ImGui::SetNextWindowPos(ImVec2(100, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(280, 150), ImGuiCond_FirstUseEver);
	ImGui::Begin("Example settings");
	ImGui::Checkbox("Render models", &guiData.displayModels);
	ImGui::Checkbox("Display logos", &guiData.displayLogos);
	ImGui::Checkbox("Display background", &guiData.displayBackground);
	ImGui::Checkbox("Animate light", &guiData.animateLight);
	ImGui::SliderFloat("Light speed", &guiData.lightSpeed, 0.1f, 1.0f);
	ImGui::End();
	// Add ImGui's example window
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::ShowDemoWindow();
	// The ImGui::Render method will generate the draw command lists we need to fill the index and vertex buffers
	ImGui::Render();
}

void VkImgui::updateBuffer(uint32_t frameIndex)
{
	ImDrawData *imDrawData = ImGui::GetDrawData();

	VkDeviceSize  vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
	VkDeviceSize  indexBufferSize  = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);
	FrameObjects &currentFrame     = frameObjects[frameIndex];
	if ((vertexBufferSize == 0) || (indexBufferSize == 0))
	{
		//currentFrame.vertexCount = 0;
		//currentFrame.indexCount  = 0;

		currentFrame.vertix.count = 0;
		currentFrame.indix.count  = 0;
		return;
	}

	// Update buffers only if it's either empty or too small to hold the required number of indices or vertices

	// Vertex buffer
	if ((currentFrame.vertix.buffer == VK_NULL_HANDLE) || (imDrawData->TotalVtxCount > currentFrame.vertix.count))
	{
		currentFrame.vertix.CleanUp(device);

		device->CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, currentFrame.vertix.buffer, currentFrame.vertix.memory, VK_SHARING_MODE_EXCLUSIVE, windows->GetSurfaceRef());

		currentFrame.vertix.count = imDrawData->TotalVtxCount;
	}

	// Index buffer
	if ((currentFrame.indix.buffer == VK_NULL_HANDLE) || (imDrawData->TotalIdxCount > currentFrame.indix.count))
	{
		currentFrame.indix.CleanUp(device);
		(device->CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, currentFrame.indix.buffer, currentFrame.indix.memory, VK_SHARING_MODE_EXCLUSIVE, windows->GetSurfaceRef()));
		currentFrame.indix.count = imDrawData->TotalIdxCount;
	}

	// Upload data
	ImDrawVert *vtxDst = (ImDrawVert *) currentFrame.vertix.mapped;
	ImDrawIdx * idxDst = (ImDrawIdx *) currentFrame.indix.mapped;

	for (int n = 0; n < imDrawData->CmdListsCount; n++)
	{
		const ImDrawList *cmd_list = imDrawData->CmdLists[n];
		memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtxDst += cmd_list->VtxBuffer.Size;
		idxDst += cmd_list->IdxBuffer.Size;
	}

	// As we don't require a host coherent memory type, flushes are required to make writes visible to the GPU
	//currentFrame.vertexBuffer.flush();
	VkMappedMemoryRange mappedRangeVer = {};
	mappedRangeVer.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRangeVer.memory              = currentFrame.vertix.memory;
	mappedRangeVer.offset              = 0;
	mappedRangeVer.size                = vertexBufferSize;

	if (vkFlushMappedMemoryRanges(device->GetLogicalDeviceRef(), 1, &mappedRangeVer) != VK_SUCCESS)
	{
		std::runtime_error("can't flush memory!");
	}

	VkMappedMemoryRange mappedRangeInd = {};
	mappedRangeInd.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRangeInd.memory              = currentFrame.indix.memory;
	mappedRangeInd.offset              = 0;
	mappedRangeInd.size                = indexBufferSize;
	if (vkFlushMappedMemoryRanges(device->GetLogicalDeviceRef(), 1, &mappedRangeInd) != VK_SUCCESS)
	{
		std::runtime_error("can't flush memory!");
	}
}

void VkImgui::InitDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding setLayoutBinding{};
	setLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	setLayoutBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
	setLayoutBinding.binding         = 0;
	setLayoutBinding.descriptorCount = 1;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings    = &setLayoutBinding;
	descriptorSetLayoutCreateInfo.bindingCount = 1;

	if (vkCreateDescriptorSetLayout(device->GetLogicalDeviceRef(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		std::runtime_error("failed to create descriptor set layout!");
	}
}

void VkImgui::Vertices::CleanUp(VkDeviceManager *para_device_manager)
{
	if (buffer)
	{
		vkDestroyBuffer(para_device_manager->GetLogicalDeviceRef(), buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(para_device_manager->GetLogicalDeviceRef(), memory, nullptr);
	}
}

void VkImgui::Indices::CleanUp(VkDeviceManager *para_device_manager)
{
	if (buffer)

	{
		vkDestroyBuffer(para_device_manager->GetLogicalDeviceRef(), buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(para_device_manager->GetLogicalDeviceRef(), memory, nullptr);
	}
}
