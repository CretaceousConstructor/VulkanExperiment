#include "PreprocessPass.h"

PreprocessPass::PreprocessPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, SceneLoading::CommonResources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), pass0_pipeline_pack(gfx), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), command_manager(gfx.CommandMan()), common_resources(common_resources_)
{
}

void PreprocessPass::ResourceInit()
{
	/****************************************************************************/

	const auto desc_pool_size_ubo          = Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::DescriptorCount<1>);
	const auto desc_pool_size_combined_img = Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Vk::DescriptorCount<1>);

	std::vector desc_pool_sizes{desc_pool_size_ubo, desc_pool_size_combined_img};

	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, Vk::SetCount<1>);
	VK_CHECK_RESULT(vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &desc_pool_CI, nullptr, &local_descriptor_pool_))

	/****************************************************************************/
	const auto &buffer_factory  = renderpass_manager.GetBufferFactory();
	const auto &texture_factory = renderpass_manager.GetTextureFactory();

	const std::string           chiricahua_narrowPath_path = "../../data/textures/hdr/NarrowPath_3k.hdr";
	VkTextureFactory::SamplerPP sampler_ppl;
	sampler_ppl.sampler_CI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_ppl.sampler_CI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_ppl.sampler_CI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	equirectangular_map                 = texture_factory.ProduceTexture(chiricahua_narrowPath_path, VK_FORMAT_R32G32B32A32_SFLOAT, sampler_ppl);

	constexpr VkBufferCI::UniformBuffer uniform_buffer_CI;
	ubo_matrix_gpu = buffer_factory.ProduceBuffer(sizeof(UboMatrix), uniform_buffer_CI, &ubo_matrix_cpu);
	/****************************************************************************/
	TextureImgPP texture_img_PP{VK_FORMAT_R16G16B16A16_SFLOAT, extend_of_cube_color_attachment};
	texture_img_PP.default_image_CI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	texture_img_PP.default_final_layout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	cube_mappings                         = renderpass_manager.GetImageFactory().ProduceImageBundlePtr(texture_img_PP, 6);
	/****************************************************************************/
	DepthImgPP depth_img_PP{gfx};
	depth_img_PP.default_image_extent = extend_of_cube_color_attachment;
	depth_image                       = renderpass_manager.GetImageFactory().ProduceImage(depth_img_PP);
	/****************************************************************************/
	//TODO:abstraction
	VkCommandManager::CreateCommandBuffer(device_manager.GetLogicalDevice(), command_manager.graphics_command_pool, local_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);


}

void PreprocessPass::CreateDescriptorSetLayout()
{
	const auto &descriptor_manager = renderpass_manager.GetDescriptorManager();

	const auto        binding0 = Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);                  //matrix
	const auto        binding1 = Vk::GetDescriptorSetLayoutBinding(Vk::Binding<1>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);        //equirectangular_map
	const std::vector bindings{binding0, binding1};
	const auto        desc_set_CI = Vk::GetDescriptorSetLayoutCI(bindings);
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(), &desc_set_CI, nullptr, &local_descriptor_set_layout))
}

void PreprocessPass::CreateDescriptorSetsThenUpdate()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	local_descriptor_set              = desciptor_set_factory.ProduceDescriptorSet(local_descriptor_pool_, local_descriptor_set_layout);

	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *ubo_matrix_gpu, local_descriptor_set, Vk::Binding<0>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *equirectangular_map, local_descriptor_set, Vk::Binding<1>);
}

void PreprocessPass::CreateAttachments()
{
	//swapchain attachment index 0
	for (size_t i = 0; i < 6; i++)
	{
		VkAttachmentInfo color_attachment{std::vector{cube_mappings->GetImagesArray()[i]}};
		auto &           attachment_dec_color = color_attachment.attachment_description;
		attachment_dec_color.format           = VK_FORMAT_R16G16B16A16_SFLOAT;
		attachment_dec_color.samples          = VK_SAMPLE_COUNT_1_BIT;
		attachment_dec_color.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment_dec_color.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_dec_color.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment_dec_color.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment_dec_color.initialLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_attachment.inpass_layout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment_dec_color.finalLayout      = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		color_attachment.attachment_index     = static_cast<uint32_t>(i);
		color_attachment.type                 = VkAttachmentInfo::AttachmentType::ColorAttachment;
		color_attachment.clear_value.color    = {0.0f, 0.0f, 0.0f, 1.0f};

		attachments.push_back(color_attachment);
	}

	//Depth attachment index 1
	VkAttachmentInfo depth_attachment{{depth_image}};
	auto &           attachment_dec_depth = depth_attachment.attachment_description;
	attachment_dec_depth.format           = swapchain_manager.FindDepthFormat();
	attachment_dec_depth.samples          = VK_SAMPLE_COUNT_1_BIT;
	attachment_dec_depth.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_dec_depth.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_dec_depth.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_dec_depth.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	attachment_dec_depth.initialLayout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachment_dec_depth.finalLayout          = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment.inpass_layout            = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment.attachment_index         = 6;
	depth_attachment.type                     = VkAttachmentInfo::AttachmentType::DepthAttachment;
	depth_attachment.clear_value.depthStencil = {1.0f, 0};
	//TODO:COPY CONTROL
	attachments.push_back(depth_attachment);
}

void PreprocessPass::CreateGraphicsPipelineLayout()
{
}

void PreprocessPass::CompileShaders()
{
	system("..\\..\\data\\shaderbat\\SceneLoadingCompile.bat");

	vertex_shader   = std::make_shared<VkShaderWrapper>(VK_SHADER_STAGE_VERTEX_BIT, std::string("../../data/shaders/SceneLoading/PreProcessPass_vertex_shader.spv"), gfx);
	fragment_shader = std::make_shared<VkShaderWrapper>(VK_SHADER_STAGE_FRAGMENT_BIT, std::string("../../data/shaders/SceneLoading/PreProcessPass_fragment_shader.spv"), gfx);
}

void PreprocessPass::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK_RESULT(vkBeginCommandBuffer(local_command_buffer, &beginInfo))



	VkRenderingInfo rendering_info{};
	rendering_info.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering_info.renderArea.offset = {0, 0};
	rendering_info.renderArea.extent = VkExtent2D{extend_of_cube_color_attachment.width, extend_of_cube_color_attachment.height};
	rendering_info.layerCount        = 1;
	rendering_info.pNext             = nullptr;

	constexpr uint32_t                     attachment_index{0};
	std::vector<VkRenderingAttachmentInfo> color_attachment_infos;
	VkRenderingAttachmentInfo              depth_attachment_info;
	VkRenderingAttachmentInfo              stensil_attachment_info;
	for (auto &attchment : attachments)
	{
		if (VkAttachmentInfo::AttachmentType::ColorAttachment == attchment.type)
		{
			color_attachment_infos.push_back(attchment.GetRenderingAttachmentInfo(attachment_index));
		}

		if (VkAttachmentInfo::AttachmentType::DepthAttachment == attchment.type)
		{
			depth_attachment_info           = attchment.GetRenderingAttachmentInfo(attachment_index);
			rendering_info.pDepthAttachment = &depth_attachment_info;
		}
	}

	rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
	rendering_info.pColorAttachments    = color_attachment_infos.data();


	LayoutTransitionStartOfRendering(local_command_buffer);
	vkCmdBeginRendering(local_command_buffer, &rendering_info);
}

void PreprocessPass::ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	const auto &des_man      = renderpass_manager.GetDescriptorManager();
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const std::vector<VkDescriptorSetLayout>           common_layouts{local_descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{vertex_shader->GetShaderStageCI(), fragment_shader->GetShaderStageCI()};

	pass0_pipeline_pack.shader_stage_CI                 = shader_stages;
	pass0_pipeline_pack.attachment_infos                = attachments;
	pass0_pipeline_pack.depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	//TODO: abstrction
	VkViewport viewport{};
	viewport.x        = 0.0f;
	viewport.y        = static_cast<float>(extend_of_cube_color_attachment.height);
	viewport.width    = static_cast<float>(extend_of_cube_color_attachment.width);
	viewport.height   = -static_cast<float>(extend_of_cube_color_attachment.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkExtent2D scissor_extend;
	scissor_extend.width  = extend_of_cube_color_attachment.width;
	scissor_extend.height = extend_of_cube_color_attachment.height;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = scissor_extend;

	pass0_pipeline_pack.view_port_scissor_pair.first.clear();
	pass0_pipeline_pack.view_port_scissor_pair.second.clear();
	pass0_pipeline_pack.view_port_scissor_pair.first.push_back(viewport);
	pass0_pipeline_pack.view_port_scissor_pair.second.push_back(scissor);

	VkPushConstantRange constant_range{};
	constant_range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	constant_range.size       = sizeof(int);
	constant_range.offset     = sizeof(glm::mat4);

	common_resources.sky_box->ProcessMaterial(common_layouts, pass0_pipeline_pack, pipe_builder, {constant_range});


	vkCmdBindDescriptorSets(local_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, common_resources.sky_box->GetPipelineLayout(), 0, 1, &local_descriptor_set, 0, NULL);

	for (size_t i = 0; i < 6; i++)
	{
		int id = (int) i;
		vkCmdPushConstants(local_command_buffer, common_resources.sky_box->GetPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), sizeof(int), &id);
		common_resources.sky_box->Draw(local_command_buffer);
	}

	common_resources.sky_box->CleanUpMaterial();


}

void PreprocessPass::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{


	vkCmdEndRendering(local_command_buffer);
	LayoutTransitionEndOfRendering(local_command_buffer);

	VK_CHECK_RESULT(vkEndCommandBuffer(local_command_buffer))




	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//const VkSemaphore              waitSemaphores[] = {image_available_semaphores->GetOne(currentFrame)};        //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image还需要presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	//constexpr VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	//submit_info.waitSemaphoreCount                  = 1;
	//submit_info.pWaitSemaphores                     = waitSemaphores;
	//submit_info.pWaitDstStageMask                   = waitStages;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores    = nullptr;
	submit_info.pWaitDstStageMask  = nullptr;

	const std::array<VkCommandBuffer, 1> submit_command_buffers = {local_command_buffer};

	submit_info.commandBufferCount = submit_command_buffers.size();
	submit_info.pCommandBuffers = submit_command_buffers.data();

	//const VkSemaphore signalSemaphores[] = {render_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	//submit_info.signalSemaphoreCount     = 1;
	//submit_info.pSignalSemaphores        = signalSemaphores;

	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores    = nullptr;

	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &submit_info, nullptr))
	VK_CHECK_RESULT(vkQueueWaitIdle(device_manager.GetGraphicsQueue()))


}

void PreprocessPass::UpdateResources(size_t currentImage)
{}

void PreprocessPass::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer) const
{
}

void PreprocessPass::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer) const
{
}
