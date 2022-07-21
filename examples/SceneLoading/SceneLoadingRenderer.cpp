#include "SceneLoadingRenderer.h"

using namespace SceneLoading;
void SceneLoadingRenderer::PrepareCommonModels()
{
	const auto &model_factory = render_pass_manager.GetModelFactory();

	const std::string damaged_helmet_path = "../../data/models/SciFiHelmet/SciFiHelmet.gltf";
	common_resources.damaged_helmet       = model_factory.GetGltfModel<PbrMaterialMetallic>(damaged_helmet_path, VkModelFactory::LoadingOption::None, 0);


	const std::string box_path = "../../data/models/cube.gltf";
	common_resources.sky_box   = model_factory.GetGltfModel<PbrMaterialMetallic>(box_path, VkModelFactory::LoadingOption::None, 0);

	//auto                  data = Geometry::CreateSphere(.2f, 20, 40, glm::vec4(1.f, 1.f, 1.f, 1.f));
	//std::vector<Vertex>   vertices;

	//std::vector<uint32_t> indices;
	//for (auto &v : data.vertexVec)
	//{
	//	vertices.push_back(Vertex(v.pos));
	//}
	//indices = std::move(data.indexVec);

	//light_indicator = std::make_unique<VkModel<Vertex>>(vertices, indices, device_manager, window->GetSurfaceRef(), transfer_command_buffer);
	//light_indicator->GetTransform().SetPosition(ubo_vs_scene.light_pos);







}
//
void SceneLoadingRenderer::CommandBufferRecording()
{
	auto &graphics_command_buffers = command_manager.GetGraphicsCommandBuffers();

	VkCommandManager::BeginCommandBuffers(graphics_command_buffers);

	renderpasses[0]->BeginRenderpass(graphics_command_buffers);
	renderpasses[0]->ExecuteRenderpass(graphics_command_buffers);
	renderpasses[0]->EndRenderpass(graphics_command_buffers);

	VkCommandManager::EndCommandBuffers(graphics_command_buffers);
}

void SceneLoadingRenderer::CreateCommonUniformBuffer()
{
	auto &buffer_factory{render_pass_manager.GetBufferFactory()};

	//GPU SIDE
	constexpr VkBufferCI::UniformBuffer unim_buf_para_pack;
	common_resources.ubo_matrix_gpu = buffer_factory.ProduceBufferBundlePtr(sizeof(Common::UboMatrix), swapchain_manager.GetSwapImageCount(), unim_buf_para_pack);
}

void SceneLoadingRenderer::CreateCommonDescriptorPool()
{
	// Create the global descriptor pool
	//TODO:这里的分配数目
	auto &des_man = render_pass_manager.GetDescriptorManager();
	{
		std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs{
		    std::pair{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3}};
		const auto max_sets = swapchain_manager.GetSwapImageCount();
		des_man.AddDescriptorPool(SceneLoading::pool_main_thread, std::move(info_pairs), max_sets);
	}
}

void SceneLoadingRenderer::InitRenderpasses()
{
	//renderpasses.push_back(std::make_shared<Renderpass0>(gfx, render_pass_manager, common_resources));
	//renderpasses.back()->Init();
	renderpasses.push_back(std::make_shared<PreprocessPass>(gfx, render_pass_manager, common_resources));
	renderpasses.back()->Init();


}

void SceneLoadingRenderer::InitSynObjects()
{
	const auto &syn_obj_factory{render_pass_manager.GetSynOjectFactory()};

	image_available_semaphores = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	render_finished_semaphores = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	frame_fences               = syn_obj_factory.GetFenceBundle(MAX_FRAMES_IN_FLIGHT, VkSynObjectBundleBase::SyncObjCreateOption::Signaled);
	image_fences.resize(swapchain_manager.GetSwapImageCount());
}

void SceneLoadingRenderer::UpdateUniformBuffer(size_t current_image_index)
{
	//struct UBO_VS_SCENE{
	//	glm::mat4 projection;
	//	glm::mat4 view;
	//	glm::vec4 light_pos   = glm::vec4(0.0f, 2.5f, 0.0f, 1.0f);
	//	glm::vec4 view_pos;
	//} ubo_vs_scene;                   //用于顶点着色器的uniform buffer object

	common_resources.ubo_matrix_cpu.projection = camera->GetProj();
	//common_resources.ubo_matrix_cpu.view       = camera->GetView();
	common_resources.ubo_matrix_cpu.view = camera->GetViewMatrix();
	//common_resources.ubo_matrix_cpu.view_pos   = camera->GetPosition();
	common_resources.ubo_matrix_cpu.cam_pos = camera->GetEyePos();

	common_resources.ubo_matrix_gpu->GetOne(current_image_index).CopyTo(&common_resources.ubo_matrix_cpu, sizeof(common_resources.ubo_matrix_cpu));



}

void SceneLoadingRenderer::DrawFrame(float time_diff)
{
	//TODO:image使用完毕和subpass dependency的关系
	static size_t currentFrame = 0;
	////所有fence    在默认初始化（不指定初始状态）时都处于unsignaled的状态
	////所有semaphore在默认初始化（不指定初始状态）时都处于unsignaled的状态
	///
	///
	////等待frame
	vkWaitForFences(device_manager.GetLogicalDevice(), 1, &(frame_fences->GetOne(currentFrame)), VK_TRUE, UINT64_MAX);        //vkWaitForFences无限时阻塞CPU，等待fence被signal后 从 unsignaled状态 变成 signaled状态才会停止阻塞。                  To wait for one or more fences to enter the signaled state on the host,

	uint32_t imageIndex;

	//获得的下一个image 的index后，可能这个index对应的image还没有被presentation engine使用完毕，所以需要一个image_available_semaphores->GetOne(currentFrame)来   通知这一阵的GPU   已经可以用这个index对应image的渲染了。
	VkResult result = vkAcquireNextImageKHR(device_manager.GetLogicalDevice(), swapchain_manager.GetSwapChain(), UINT64_MAX, image_available_semaphores->GetOne(currentFrame), nullptr, &imageIndex);        //As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.得到下一个可以使用的image的index，但是这个image可能还没用完，这里获得的imageIndex对应的image很有可能是在最短的时间内被某一帧使用完毕的那一个，由vulkan实现具体决定

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

	//images_fence大小为3在这个例子里，注意它的大小不是2(MAX_FRAMES_IN_FLIGHT)
	//防止image正在被GPU使用
	if (image_fences[imageIndex] != nullptr)
	{
		//	images_inflight[imageIndex] 不是 nullptr，说明某一帧正在占用GPU资源(和image有关的资源)这些GPU资源正在被以imageIndex为下标的image使用，那么我们就要等待。
		//TODO:目前的设计是每一个image都有一套资源，但如果 max_frame_in_flight小于swap chain image的话，只需要max_frame_in_flight套资源就可以了，但是如果这么做就需要记录到底是哪一帧在使用images_inflight[imageIndex]关联的资源，从而在下面的WaitForFences语句以后就可以使用空出来的资源
		//	无限等待fence,
		vkWaitForFences(device_manager.GetLogicalDevice(), 1, &image_fences[imageIndex], VK_TRUE, UINT64_MAX);
	}

	image_fences[imageIndex] = frame_fences->GetOne(currentFrame);        //等待完images毕后，让接下来会被GPU用来填色的image被currentFrame所占有(表示currentFrame这一帧的GPU资源正在被index为imageIndex的image占用)，目前inflight_fences[currentFrame]处于signled的状态。

	////可以看到这里graphics_command_buffers[imageIndex]使用完以后，inflight_fences_frame[currentFrame]会被signal（读vkQueueSubmit的定义），那么怎么确定这里的graphics_command_buffers[imageIndex]已经被前几帧使用完毕了呢？

	////假设这样的渲染流程，

	////===========================================================================================================================================================
	////第一帧            currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](第一次使用不用等)，
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（但第一次使用时，images_fences[imageIndex 0]为nullptr），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用
	////===========================================================================================================================================================
	////第二帧            currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](第一次用不用等)，
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（但第一次使用时，images_fences[imageIndex 2]为nullptr），
	////3.赋值:        images_fences[imageIndex 2（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1];
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用
	////===========================================================================================================================================================

	////第三帧（第一种可能) currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]渲染使用结束后对它的等待也就结束了),
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为1（由实现决定)]（但第一次使用时，images_fences[imageIndex 1]为nullptr），
	////3.赋值:        images_fences[imageIndex 1（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 1]被使用

	////第三帧(第二种可能) currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]渲染使用结束后对它的等待也就结束了),
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（images_fences[0]之前被frame_fences[currentFrame 0]赋值，这里就是等待frame_fences[currentFrame 0]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用

	////第三帧(第三种可能) currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]渲染使用结束后对它的等待也就结束了),
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（images_fences[2]之前被frame_fences[currentFrame 1]赋值，这里就是等待frame_fences[currentFrame 1]），
	////3.赋值:        images_fences[imageIndex 2（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用

	////===========================================================================================================================================================

	////#1（假设为第三帧为第一种可能）
	////第四帧（第一种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（之前被frame_fences[currentFrame 0]赋值），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用中

	////#1（假设为第三帧为第一种可能）
	////第四帧（第二种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（之前被frame_fences[currentFrame 1]赋值），
	////3.赋值:        images_fences[imageIndex 2（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用中

	////#1（假设为第三帧为第一种可能）
	////第四帧（第三种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为1（由实现决定)]（之前被frame_fences[currentFrame 0]赋值），
	////3.赋值:        images_fences[imageIndex 1（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 1]被使用中

	////===========================================================================================================================================================

	////#1（假设为第三帧为第二种可能）
	////第四帧（第一种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（images_fences[0]之前被frame_fences[currentFrame 0]赋值），这里就是等待frame_fences[currentFrame 0]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用中

	////#1（假设为第三帧为第二种可能）
	////第四帧（第二种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（images_fences[2]之前被frame_fences[currentFrame 1]赋值），这里就是等待frame_fences[currentFrame 1]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用中

	////#1（假设为第三帧为第二种可能）
	////第四帧（第二种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为1（由实现决定)]（但第一次使用时，images_fences[imageIndex 1]为nullptr）
	////3.赋值:        images_fences[imageIndex 1（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 1]被使用中

	////===========================================================================================================================================================

	////#1（假设为第三帧为第三种可能）
	////第四帧（第一种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（之前被frame_fences[currentFrame 0]赋值），这里就是等待frame_fences[currentFrame 0]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用中

	////===========================================================================================================================================================
	//因为上面有image_fences[imageIndex] = frame_fences[currentFrame];  所以这时候image_fences[imageIndex]和 frame_fences[currentFrame]会有同样的状态;并且应该同时为unsignaled状态

	//frame_fences[currentFrame]; 这时候image_fences[imageIndex]和 frame_fences[currentFrame]会有同样的状态;并且应该同时为unsignaled状态，那么reset为unsignal状态表示现在我要占用了！
	vkResetFences(device_manager.GetLogicalDevice(), 1, &frame_fences->GetOne(currentFrame));        //To set the state of fences to unsignaled from the host side
	//vkQueueSubmit:   fence(last parameter is an optional handle to a fence to be signaled once all submitted command buffers have completed execution. If fence is not VK_NULL_HANDLE, it defines a fence signal operation.当command buffer中的命令执行结束以后，也就是GPU渲染完毕以后

	//UI Reset command buffers
	imgui_UI.ResetCommandPoolAndBeginCommandBuffer(imageIndex);

	//UI Describing UI
	imgui_UI.DescribingUI();

	UpdateCamera(time_diff);

	//UI command buffer recording
	imgui_UI.RenderingCommandRecord(imageIndex);

	VkSubmitInfo submit_info{};
	submit_info.sType                               = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	const VkSemaphore              waitSemaphores[] = {image_available_semaphores->GetOne(currentFrame)};        //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image还需要presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	constexpr VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submit_info.waitSemaphoreCount                  = 1;
	submit_info.pWaitSemaphores                     = waitSemaphores;
	submit_info.pWaitDstStageMask                   = waitStages;

	const auto graphics_command_buffer = command_manager.GetGraphicsCommandBuffers()[imageIndex];
	const auto UI_command_buffer       = imgui_UI.GetCommandBuffer(imageIndex);

	const std::array<VkCommandBuffer, 2> submit_command_buffers = {graphics_command_buffer, UI_command_buffer};

	submit_info.commandBufferCount = submit_command_buffers.size();
	//&graphics_command_buffers[imageIndex] 用的frame buffer就是swap image[imageIndex]
	submit_info.pCommandBuffers = submit_command_buffers.data();

	const VkSemaphore signalSemaphores[] = {render_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	submit_info.signalSemaphoreCount     = 1;
	submit_info.pSignalSemaphores        = signalSemaphores;

	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &submit_info, frame_fences->GetOne(currentFrame)))

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;

	const VkSwapchainKHR swapChains[] = {swapchain_manager.GetSwapChain()};
	presentInfo.pSwapchains           = swapChains;
	presentInfo.swapchainCount        = 1;
	presentInfo.pImageIndices         = &imageIndex;
	presentInfo.pResults              = nullptr;        // Optional

	result = vkQueuePresentKHR(device_manager.GetPresentQueue(), &presentInfo);

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

void SceneLoadingRenderer::UpdateCamera(float dt)
{
	////TODO:用命令模式优化
	static bool stop_cam = false;

	if (keyboard->GetIsKeyDown(GLFW_KEY_G))
	{
		stop_cam = (!stop_cam);
	}

	if (stop_cam)
	{
		return;
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(const_cast<GLFWwindow *>(window.GetWindowPtr()), GLFW_TRUE);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_W))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Forward, dt);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_S))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Backward, dt);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_A))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Left, dt);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_D))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::Right, dt);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_Q))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::RollLeft, dt);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_E))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::RollRight, dt);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_Z))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::ZoomIn, dt);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_C))
	{
		camera->ProcessKeyboard(FirstPersonCamera::Camera_Movement::ZoomOut, dt);
	}

	camera->ProcessMouseMovement(mouse->GetYawDiff(), mouse->GetPitchDiff());
	camera->ProcessMouseScroll(mouse->GetMouseScroll());
}

//void SceneLoadingRenderer::CreateDescriptorSetsThenUpdate()
//{
//	descriptor_sets_for_matrices.resize(swapchain_manager->GetSwapImageCount());
//	{
//		//set = 0
//		//Descriptor sets for vs matrix
//		for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
//		{
//			//ALLOCATE DESCRIPTORS
//			VkDescriptorSetAllocateInfo allocInfoWrite{};
//			allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//			allocInfoWrite.descriptorPool     = descriptor_pool;
//			allocInfoWrite.descriptorSetCount = 1;
//			allocInfoWrite.pSetLayouts        = &descriptor_set_layout_matrices;
//
//			if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &descriptor_sets_for_matrices[i]) != VK_SUCCESS)
//			{
//				throw std::runtime_error("failed to allocate descriptor sets!");
//			}
//			//DESCRIPTORS INFOS
//			VkDescriptorBufferInfo buffer_info_write_subpass0{};
//			buffer_info_write_subpass0.buffer = uniform_buffers[i].buffer;
//			buffer_info_write_subpass0.offset = 0;
//			buffer_info_write_subpass0.range  = sizeof(ubo_vs_scene);
//
//			//UPDATE DESCRIPTORS
//			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
//			VkWriteDescriptorSet              temp_writeDescriptorSet{};
//			/*
//				Binding 0: VS Object matrices Uniform buffer
//			*/
//			temp_writeDescriptorSet.sType          = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			temp_writeDescriptorSet.dstSet         = descriptor_sets_for_matrices[i];
//			temp_writeDescriptorSet.dstBinding     = 0;
//			temp_writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//			//Otherwise, descriptorCount is one of the number of elements in pImageInfo or the number of elements in pBufferInfo
//			temp_writeDescriptorSet.descriptorCount = 1;
//			temp_writeDescriptorSet.pBufferInfo     = &buffer_info_write_subpass0;
//			temp_writeDescriptorSet.dstArrayElement = 0;
//
//			writeDescriptorSets.push_back(temp_writeDescriptorSet);
//
//			vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
//		}
//		// set=1
//		// Descriptor sets for materials of models
//
//		for (auto &material : gltf_scene_sponza->materials)
//		{
//			//ALLOCATE DESCRIPTORS
//			VkDescriptorSetAllocateInfo allocInfoWrite{};
//			allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//			allocInfoWrite.descriptorPool     = descriptor_pool;
//			allocInfoWrite.descriptorSetCount = 1;
//			allocInfoWrite.pSetLayouts        = &descriptor_set_layout_textures;
//
//			if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &material.descriptorSet) != VK_SUCCESS)
//			{
//				throw std::runtime_error("failed to allocate descriptor sets!");
//			}
//			//DESCRIPTOR INFOS
//
//			//UPDATE DESCRIPTORS
//			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
//			VkWriteDescriptorSet              temp_writeDescriptorSet{};
//
//			VkDescriptorImageInfo colorMapInfo  = gltf_scene_sponza->GetTextureDescriptorInfo(material.baseColorTextureIndex);
//			VkDescriptorImageInfo normalMapInfo = gltf_scene_sponza->GetTextureDescriptorInfo(material.normalTextureIndex);
//			/*
//				Binding 0: color mapping
//			*/
//
//			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			temp_writeDescriptorSet.dstSet          = material.descriptorSet;
//			temp_writeDescriptorSet.dstBinding      = 0;
//			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//			temp_writeDescriptorSet.pImageInfo      = &colorMapInfo;
//			temp_writeDescriptorSet.descriptorCount = 1;
//			temp_writeDescriptorSet.dstArrayElement = 0;
//
//			writeDescriptorSets.push_back(temp_writeDescriptorSet);
//
//			/*
//				Binding 1: normal mapping
//			*/
//			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			temp_writeDescriptorSet.dstSet          = material.descriptorSet;
//			temp_writeDescriptorSet.dstBinding      = 1;
//			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//			temp_writeDescriptorSet.pImageInfo      = &normalMapInfo;
//			temp_writeDescriptorSet.descriptorCount = 1;
//			temp_writeDescriptorSet.dstArrayElement = 0;
//
//			writeDescriptorSets.push_back(temp_writeDescriptorSet);
//
//			//UPDATE DESCRIPTOR SET
//			vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
//		}
//	}
//}
//
//void SceneLoadingRenderer::CreateGraphicsPipelineLayout()
//{
//	{
//		std::vector<VkDescriptorSetLayout> setLayouts = {descriptor_set_layout_matrices, descriptor_set_layout_textures};
//		VkPipelineLayoutCreateInfo         pipelineLayoutInfo_subpass0{};
//		pipelineLayoutInfo_subpass0.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//		pipelineLayoutInfo_subpass0.setLayoutCount = setLayouts.size();
//		pipelineLayoutInfo_subpass0.pSetLayouts    = setLayouts.data();
//
//		//TODO: testing multiple push constants and how to access it
//		// We will use push constants to push the local matrices of a primitive to the vertex shader
//		VkPushConstantRange pushConstantRange{};
//		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//		pushConstantRange.size       = sizeof(glm::mat4);
//		pushConstantRange.offset     = 0;
//
//		pipelineLayoutInfo_subpass0.pushConstantRangeCount = 1;                         // Optional
//		pipelineLayoutInfo_subpass0.pPushConstantRanges    = &pushConstantRange;        // Optional
//
//		if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass0, nullptr, &pipeline_layout) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create pipeline layout!");
//		}
//	}
//
//	{
//		std::vector<VkDescriptorSetLayout> setLayouts = {descriptor_set_layout_matrices};
//		VkPipelineLayoutCreateInfo         pipelineLayoutInfo_subpass0{};
//		pipelineLayoutInfo_subpass0.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//		pipelineLayoutInfo_subpass0.setLayoutCount = setLayouts.size();
//		pipelineLayoutInfo_subpass0.pSetLayouts    = setLayouts.data();
//
//		VkPushConstantRange pushConstantRange{};
//		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//		pushConstantRange.size       = sizeof(glm::mat4);
//		pushConstantRange.offset     = 0;
//
//		pipelineLayoutInfo_subpass0.pushConstantRangeCount = 1;                         // Optional
//		pipelineLayoutInfo_subpass0.pPushConstantRanges    = &pushConstantRange;        // Optional
//
//		if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass0, nullptr, &pipeline_layout_light_indicator) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create pipeline layout!");
//		}
//	}
//}
//void SceneLoadingRenderer::CreateGraphicsPipeline()
//{
//	system("..\\..\\data\\shaderbat\\SceneLoadingCompile.bat");
//
//	VkShaderManager vertex_shader(std::string("../../data/shaders/SceneLoading/SceneLoading_vertex_shader.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
//	VkShaderManager fragment_shader(std::string("../../data/shaders/SceneLoading/SceneLoading_fragment_shader.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());
//
//	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info = {vertex_shader.GetVkPipelineShaderStageCreateInfo(), fragment_shader.GetVkPipelineShaderStageCreateInfo()};
//
//	//TODO:需要更多的abstraction
//	VkVertexInputBindingDescription bindingDescription0{};
//
//	bindingDescription0.binding   = 0;
//	bindingDescription0.stride    = sizeof(GltfModel::Vertex);
//	bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//	std::vector<VkVertexInputBindingDescription> VIBDS = {bindingDescription0};
//
//	auto attributeDescriptions = GltfModel::Vertex::GetAttributeDescriptions();
//
//	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
//	vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//	vertex_input_info.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
//	vertex_input_info.pVertexBindingDescriptions      = VIBDS.data();
//	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//	vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();
//
//	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//	inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//	inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//	inputAssembly.flags                  = 0;
//	inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//	VkViewport       viewport{};
//	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
//	viewport.x                            = 0.0f;
//	viewport.y                            = (float) extend_of_swap_image.height;
//	viewport.width                        = (float) extend_of_swap_image.width;
//	viewport.height                       = -(float) extend_of_swap_image.height;
//	viewport.minDepth                     = 0.0f;
//	viewport.maxDepth                     = 1.0f;
//
//	VkExtent2D swapChainExtent;
//	swapChainExtent.height = extend_of_swap_image.height;
//	swapChainExtent.width  = extend_of_swap_image.width;
//
//	VkRect2D scissor{};
//	scissor.offset = {0, 0};
//	scissor.extent = swapChainExtent;
//
//	VkPipelineViewportStateCreateInfo viewportState{};
//	viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//	viewportState.viewportCount = 1;
//	viewportState.pViewports    = &viewport;
//	viewportState.scissorCount  = 1;
//	viewportState.pScissors     = &scissor;
//
//	VkPipelineRasterizationStateCreateInfo rasterizer{};
//	rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//	rasterizer.depthClampEnable        = VK_FALSE;
//	rasterizer.rasterizerDiscardEnable = VK_FALSE;
//	rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
//	rasterizer.lineWidth               = 1.f;
//	rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
//	rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//	rasterizer.depthBiasEnable         = VK_FALSE;
//	rasterizer.depthBiasConstantFactor = 0.0f;        // Optional
//	rasterizer.depthBiasClamp          = 0.0f;        // Optional
//	rasterizer.depthBiasSlopeFactor    = 0.0f;        // Optional
//	rasterizer.flags                   = 0;
//
//	VkPipelineMultisampleStateCreateInfo multisampling{};
//	multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//	multisampling.sampleShadingEnable   = VK_FALSE;
//	multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
//	multisampling.minSampleShading      = 1.0f;            // Optional
//	multisampling.pSampleMask           = nullptr;         // Optional
//	multisampling.alphaToCoverageEnable = VK_FALSE;        // Optional
//	multisampling.alphaToOneEnable      = VK_FALSE;        // Optional
//
//	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//	colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	colorBlendAttachment.blendEnable         = VK_FALSE;
//	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
//	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
//	colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
//	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
//	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
//	colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional
//
//	VkPipelineColorBlendStateCreateInfo colorBlending{};
//	colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//	colorBlending.logicOpEnable     = VK_FALSE;
//	colorBlending.logicOp           = VK_LOGIC_OP_COPY;        // Optional
//	colorBlending.attachmentCount   = 1;
//	colorBlending.pAttachments      = &colorBlendAttachment;
//	colorBlending.blendConstants[0] = 0.0f;        // Optional
//	colorBlending.blendConstants[1] = 0.0f;        // Optional
//	colorBlending.blendConstants[2] = 0.0f;        // Optional
//	colorBlending.blendConstants[3] = 0.0f;        // Optional
//
//	//VkDynamicState dynamicStates[] = {
//	//	VK_DYNAMIC_STATE_VIEWPORT,
//	//	VK_DYNAMIC_STATE_LINE_WIDTH
//	//};
//
//	//
//	VkPipelineDynamicStateCreateInfo dynamicState{};
//	dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//	dynamicState.dynamicStateCount = 0;
//	dynamicState.pDynamicStates    = VK_NULL_HANDLE;
//
//	VkPipelineDepthStencilStateCreateInfo depthStencil_supass0{};
//	depthStencil_supass0.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//	depthStencil_supass0.depthTestEnable       = VK_TRUE;
//	depthStencil_supass0.depthWriteEnable      = VK_TRUE;
//	depthStencil_supass0.depthCompareOp        = VK_COMPARE_OP_LESS;
//	depthStencil_supass0.back.compareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
//	depthStencil_supass0.depthBoundsTestEnable = VK_FALSE;
//	depthStencil_supass0.minDepthBounds        = 0.0f;        // Optional
//	depthStencil_supass0.maxDepthBounds        = 1.0f;        // Optional
//	depthStencil_supass0.stencilTestEnable     = VK_FALSE;
//	//depthStencil_supass0.front = {}; // Optional
//	//depthStencil_supass0.back = {}; // Optional
//
//	//typedef struct VkGraphicsPipelineCreateInfo {
//	//	VkStructureType                                  sType;
//	//	const void* pNext;
//	//	VkPipelineCreateFlags                            flags;
//	//	uint32_t                                         stageCount;
//	//	const VkPipelineShaderStageCreateInfo* pStages;
//	//	const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
//	//	const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
//	//	const VkPipelineTessellationStateCreateInfo* pTessellationState;
//	//	const VkPipelineViewportStateCreateInfo* pViewportState;
//	//	const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
//	//	const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
//	//	const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
//	//	const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
//	//	const VkPipelineDynamicStateCreateInfo* pDynamicState;
//	//	VkPipelineLayout                                 layout;
//	//	VkRenderPass                                     renderPass;
//	//	uint32_t                                         subpass;
//	//	VkPipeline                                       basePipelineHandle;
//	//	int32_t                                          basePipelineIndex;
//	//} VkGraphicsPipelineCreateInfo;
//
//	VkGraphicsPipelineCreateInfo pipeline_subpass0_CI{};
//
//	pipeline_subpass0_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//	pipeline_subpass0_CI.stageCount = (uint32_t) shader_stages_create_info.size();
//	pipeline_subpass0_CI.pStages    = shader_stages_create_info.data();
//
//	pipeline_subpass0_CI.pVertexInputState   = &vertex_input_info;
//	pipeline_subpass0_CI.pInputAssemblyState = &inputAssembly;
//	pipeline_subpass0_CI.pViewportState      = &viewportState;
//	pipeline_subpass0_CI.pRasterizationState = &rasterizer;
//	pipeline_subpass0_CI.pMultisampleState   = &multisampling;
//	pipeline_subpass0_CI.pDepthStencilState  = &depthStencil_supass0;
//
//	pipeline_subpass0_CI.pColorBlendState = &colorBlending;
//	pipeline_subpass0_CI.pDynamicState    = nullptr;        // Optional
//
//	pipeline_subpass0_CI.layout = pipeline_layout;
//
//	pipeline_subpass0_CI.renderPass = render_pass;
//	pipeline_subpass0_CI.subpass    = 0;        // index
//
//	pipeline_subpass0_CI.basePipelineIndex  = -1;
//	pipeline_subpass0_CI.basePipelineHandle = VK_NULL_HANDLE;
//
//	for (auto &material : gltf_scene_sponza->materials)
//	{
//		struct MaterialSpecializationData
//		{
//			bool  alphaMask;
//			float alphaMaskCutoff;
//		} material_specialization_data;
//
//		material_specialization_data.alphaMask       = material.alphaMode == "MASK";
//		material_specialization_data.alphaMaskCutoff = material.alphaCutOff;
//
//		//Constant fragment shader material parameters will be set using specialization constants
//		//ENTRIEs
//		std::vector<VkSpecializationMapEntry> specialization_map_entries;
//		VkSpecializationMapEntry              specialization_map_entry_temp{};
//		specialization_map_entry_temp.constantID = 0;
//		specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMask);
//		specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMask);
//		specialization_map_entries.push_back(specialization_map_entry_temp);
//		specialization_map_entry_temp.constantID = 1;
//		specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMaskCutoff);
//		specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMaskCutoff);
//		specialization_map_entries.push_back(specialization_map_entry_temp);
//
//		//INFOs
//		VkSpecializationInfo specializationInfo{};
//		specializationInfo.mapEntryCount = static_cast<uint32_t>(specialization_map_entries.size());
//		specializationInfo.pMapEntries   = specialization_map_entries.data();
//		specializationInfo.dataSize      = sizeof(material_specialization_data);
//		specializationInfo.pData         = &material_specialization_data;
//
//		shader_stages_create_info[1].pSpecializationInfo = &specializationInfo;
//		// For double sided materials, culling will be disabled
//		rasterizer.cullMode = material.doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
//
//		if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), pipeline_cache, 1, &pipeline_subpass0_CI, nullptr, &material.pipeline) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create graphics pipeline!");
//		}
//	}
//
//	CreateLightIndicatorPipeline();
//}

SceneLoadingRenderer::SceneLoadingRenderer(VkGraphicsComponent &gfx_) :
    BaseRenderer(gfx_), render_pass_manager(gfx), imgui_UI(gfx)
{
}

//void SceneLoadingRenderer::CreatePipelineCache()
//{
//	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
//	pipelineCacheCreateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
//	if (vkCreatePipelineCache(device_manager->GetLogicalDeviceRef(), &pipelineCacheCreateInfo, nullptr, &pipeline_cache) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create Pipeline Cache!");
//	}
//}
//
//
void SceneLoadingRenderer::SetUpUserInput()
{
	std::vector<int> tracked_keys = {GLFW_KEY_ESCAPE, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_G, GLFW_KEY_UP, GLFW_KEY_DOWN};
	keyboard                      = std::make_unique<KeyBoardInputManager>(tracked_keys);
	keyboard->SetupKeyInputs(window.GetWindowPtr());

	mouse = std::make_unique<MouseInputManager>(gfx);
}

void SceneLoadingRenderer::CreateCamera()
{
	camera = std::make_unique<FirstPersonCamera>(glm::vec3(0.f, 0.f, 1.f));

	camera->SetFrustum(glm::radians(60.f), swapchain_manager.GetSwapChainImageExtent().width / (float) swapchain_manager.GetSwapChainImageExtent().height, 0.1f, 256.f);
	//camera->LookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0., 1., 0.));
}
//
void SceneLoadingRenderer::CreateCommomAttachmentImgs()
{
	CreateDepthImages();
	CreateSwapchainImages();
}
//

void SceneLoadingRenderer::CreateCommonTextureImgs()
{
	//const auto &      texture_factory       = render_pass_manager.GetTextureFactory();


	//const std::string chiricahua_narrowPath_path = "../../data/textures/hdr/NarrowPath_3k.hdr";
	//const VkTextureFactory::SamplerPP sampler_ppl;
	//common_resources.hdr_environment_map = texture_factory.ProduceTexture(chiricahua_narrowPath_path,VK_FORMAT_R32G32B32A32_SFLOAT,sampler_ppl);







}
//
void SceneLoadingRenderer::CreateDepthImages()
{
	const DepthImgPP depth_para_pack{gfx};
	common_resources.depth_attachments = render_pass_manager.GetImageFactory().ProduceImageBundlePtr(depth_para_pack, swapchain_manager.GetSwapImageCount());
}

void SceneLoadingRenderer::CreateSwapchainImages()
{
	const SwapchainImgPP parameter_pack;
	common_resources.swapchain_images = render_pass_manager.GetImageFactory().ProduceImageBundlePtr(parameter_pack);
}
