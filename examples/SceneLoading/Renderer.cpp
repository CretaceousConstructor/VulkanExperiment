#include "Renderer.h"
using namespace Vk;
using namespace Global;

void Renderer::PrepareCommonModels()
{
	const auto &model_factory = renderpass_manager.GetModelFactory();

	//const std::string scifi_helmet_path = "../../data/models/SciFiHelmet/SciFiHelmet.gltf";
	//global_resources.scifi_helmet       = model_factory.GetGltfModel<PbrMaterialMetallic>(scifi_helmet_path, VkModelFactory::LoadingOption::None, 0);

	//const std::string box_path = "../../data/models/cube/Cube.gltf";
	//global_resources.sky_box   = model_factory.GetGltfModel<PbrMaterialMetallic>(box_path, VkModelFactory::LoadingOption::None, 1);

	const std::string sponza_path = "../../data/models/sponza/sponza.gltf";
	global_resources.sponza       = model_factory.GetGltfModel<NonPbrMaterial>(sponza_path, Vk::ModelLoadingOption::BindlessRenderingMode, 0);

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

void Renderer::CommandBufferRecording()
{
	auto &graphics_command_buffers = command_manager.GetGraphicsCommandBuffers();

	VkCommandManager::BeginCommandBuffers(graphics_command_buffers);

	for (size_t i = 0; i < renderpasses.size(); i++)
	{
		renderpasses[i]->ExecuteStatically(graphics_command_buffers);
	}

	VkCommandManager::EndCommandBuffers(graphics_command_buffers);
}

void Renderer::CreateCommonUniformBuffer()
{
	auto &buffer_factory{renderpass_manager.GetBufferFactory()};

	//GPU SIDE
	//FLLOWING SHIT IS USED BY PBR
	//{
	//	constexpr VkBufferCI::UniformBuffer unim_buf_PP;
	//	global_resources.matrix_buffer_gpu = buffer_factory.ProduceBufferPtrArray(sizeof(Global::Structure::UboMatrix), Vk::BundleSize<SWAP_IMG_COUNT>, unim_buf_PP);
	//}

	//GPU SIDE
	//{
	//	constexpr VkBufferCI::UniformBuffer unim_buf_PP;
	//	global_resources.matrix_buffer_gpu_MSAA = buffer_factory.ProduceBufferPtrArray(sizeof(MSAA::UBO), Vk::BundleSize<SWAP_IMG_COUNT>, unim_buf_PP);
	//}

	//GPU SIDE
	{
		constexpr VkBufferCI::UniformBuffer unim_buf_PP;
		global_resources.matrix_buffer_gpu_defered_rendering = buffer_factory.ProduceBufferPtrArray(sizeof(DeferedRendering::UBO), Vk::BundleSize<SWAP_IMG_COUNT>, unim_buf_PP);
	}



}

void Renderer::CreateCommonDescriptorPool()
{
	// Create the global descriptor pool
	//TODO:这里的分配数目

	//auto &des_man = renderpass_manager.GetDescriptorManager();
	//{
	//	std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs{
	//	    std::pair{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3}};
	//	const auto max_sets = swapchain_manager.GetSwapImageCount();
	//	des_man.AddDescriptorPool(Global::pool_main_thread, std::move(info_pairs), max_sets);
	//}

	//	const auto max_sets = swapchain_manager.GetSwapImageCount();

	const std::vector desc_pool_sizes{
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::DescriptorCount<SWAP_IMG_COUNT> * 20),
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Vk::DescriptorCount<SWAP_IMG_COUNT> * 20),
	};

	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, Vk::SetCount<SWAP_IMG_COUNT>);
	global_resources.pool   = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
}

void Renderer::InitRenderpasses()
{
	//use factory mode to optimize
	//renderpasses.push_back(std::make_shared<PrefilterAndLUTMapGenPass>(gfx, renderpass_manager, global_resources));
	//renderpasses.back()->Init();

	//renderpasses.push_back(std::make_shared<IrradianceMapGenPass>(gfx, renderpass_manager, global_resources));
	//renderpasses.back()->Init();

	//renderpasses.push_back(std::make_shared<PbrRenderingPass>(gfx, renderpass_manager, global_resources));
	//renderpasses.back()->Init();

	//VkRenderpassBase::ProduceRenderpass<DeferedGeometryPass>(gfx, renderpass_manager, global_resources);

	//renderpasses.push_back(std::make_shared<DeferedGeometryPass>(gfx, renderpass_manager, global_resources));
	//renderpasses.back()->Init();

	renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<DeferedGeometryPass>(gfx, renderpass_manager, global_resources));
	renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<DeferedCompositionPass>(gfx, renderpass_manager, global_resources));

	//renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<MSAAPass>(gfx, renderpass_manager, global_resources));
}

void Renderer::InitSynObjects()
{
	const auto &syn_obj_factory{renderpass_manager.GetSynOjectFactory()};

	image_available_semaphores          = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	graphics_render_finished_semaphores = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	UI_render_finished_semaphores       = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	frame_fences                        = syn_obj_factory.GetFenceBundle(MAX_FRAMES_IN_FLIGHT, VkSynObjectBundleBase::SyncObjCreateOption::Signaled);
	image_fences.resize(swapchain_manager.GetSwapImageCount());
}

void Renderer::UpdateUniformBuffer(size_t current_image_index)
{
	//struct UBO_VS_SCENE{
	//	glm::mat4 projection;
	//	glm::mat4 view;
	//	glm::vec4 light_pos   = glm::vec4(0.0f, 2.5f, 0.0f, 1.0f);
	//	glm::vec4 view_pos;
	//} ubo_vs_scene;                   //用于顶点着色器的uniform buffer object

	////FLLOWING SHIT IS USED BY PBR
	//global_resources.matrix_buffer_cpu.projection = camera->GetProjMatrix();
	//global_resources.matrix_buffer_cpu.view = camera->GetViewMatrix();
	//global_resources.matrix_buffer_cpu.cam_pos = camera->GetEyePos();
	//global_resources.matrix_buffer_gpu[current_image_index]->CopyFrom(&global_resources.matrix_buffer_cpu, sizeof(global_resources.matrix_buffer_cpu));

	global_resources.matrix_buffer_cpu_defered_rendering.projection   = camera->GetProjMatrix(Camera::ProjectionMtxSetting::ReversedZ);        //for revered Z
	global_resources.matrix_buffer_cpu_defered_rendering.view         = camera->GetViewMatrix();
	global_resources.matrix_buffer_cpu_defered_rendering.view_inverse = camera->GetInverseViewMatrix();
	global_resources.matrix_buffer_cpu_defered_rendering.cam_pos      = camera->GetEyePos();

	//COPY
	global_resources.matrix_buffer_gpu_defered_rendering[current_image_index]->CopyFrom(&global_resources.matrix_buffer_cpu_defered_rendering, sizeof(global_resources.matrix_buffer_cpu_defered_rendering));

	//global_resources.matrix_buffer_cpu_MSAA.projection   = camera->GetProjMatrix(Camera::ProjectionMtxSetting::ReversedZ);        //for revered Z
	//global_resources.matrix_buffer_cpu_MSAA.view         = camera->GetViewMatrix();
	//global_resources.matrix_buffer_cpu_MSAA.view_inverse = camera->GetInverseViewMatrix();
	//global_resources.matrix_buffer_cpu_MSAA.cam_pos      = camera->GetEyePos();

	////COPY
	//global_resources.matrix_buffer_gpu_MSAA[current_image_index]->CopyFrom(&global_resources.matrix_buffer_cpu_MSAA, sizeof(global_resources.matrix_buffer_cpu_MSAA));

	//global_resources.matrix_buffer_cpu_defered_rendering.projection   = camera->GetProjMatrix(Camera::ProjectionMtxSetting::ReversedZ);        //for revered Z
	//global_resources.matrix_buffer_cpu_defered_rendering.view         = camera->GetViewMatrix();
	//global_resources.matrix_buffer_cpu_defered_rendering.view_inverse = camera->GetInverseViewMatrix();
	//global_resources.matrix_buffer_cpu_defered_rendering.cam_pos      = camera->GetEyePos();

	////COPY
	//global_resources.matrix_buffer_gpu_defered_rendering[current_image_index]->CopyFrom(&global_resources.matrix_buffer_cpu_defered_rendering, sizeof(global_resources.matrix_buffer_cpu_defered_rendering));
}

void Renderer::DrawFrame(float time_diff)
{
	DrawFrame();
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
		//TODO: recreation of swapchain
		//recreateSwapChain();
		return;
	}
	else if (result == VK_NOT_READY)
	{
		std::cout << "Rely on semophore to synchronize!" << std::endl;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap-chain image!");
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
	imgui_UI.TestDescribingUI(camera->GetEyePos());

	UpdateCamera(time_diff);

	//UI command buffer recording
	imgui_UI.RenderingCommandRecord(imageIndex);

	//**********************************************************************************************************************
	//Graphics Rendering command buffer submitting.
	VkSubmitInfo gfx_submit_info{};
	gfx_submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	const VkSemaphore              gfx_wait_semaphores[] = {image_available_semaphores->GetOne(currentFrame)};        //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image渲染完成后，即便GPU资源释放了，但是还需要进行presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	constexpr VkPipelineStageFlags gfx_wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	gfx_submit_info.waitSemaphoreCount                   = 1;
	gfx_submit_info.pWaitSemaphores                      = gfx_wait_semaphores;
	gfx_submit_info.pWaitDstStageMask                    = gfx_wait_stages;

	const auto                           graphics_command_buffer = command_manager.GetGraphicsCommandBuffers()[imageIndex];
	const std::array<VkCommandBuffer, 1> submit_command_buffers  = {graphics_command_buffer};

	gfx_submit_info.commandBufferCount = static_cast<uint32_t>(submit_command_buffers.size());
	//&graphics_command_buffers[imageIndex] 用的frame buffer就是swap image[imageIndex]
	gfx_submit_info.pCommandBuffers = submit_command_buffers.data();

	const VkSemaphore gfx_signal_semaphores[] = {graphics_render_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffers执行完以后会signal这里，随后ui_command_buffer就知道可以开始渲染UI了。
	gfx_submit_info.signalSemaphoreCount      = 1;
	gfx_submit_info.pSignalSemaphores         = gfx_signal_semaphores;

	//VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &gfx_submit_info, frame_fences->GetOne(currentFrame)))
	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &gfx_submit_info, VK_NULL_HANDLE))
	//**********************************************************************************************************************
	//UI Rendering command buffer submitting.
	VkSubmitInfo ui_submit_info{};
	const auto   UI_command_buffer                      = imgui_UI.GetCommandBuffer(imageIndex);
	ui_submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	const VkSemaphore              ui_wait_semaphores[] = {graphics_render_finished_semaphores->GetOne(currentFrame)};
	constexpr VkPipelineStageFlags ui_wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	ui_submit_info.waitSemaphoreCount                   = 1;
	ui_submit_info.pWaitSemaphores                      = ui_wait_semaphores;
	ui_submit_info.pWaitDstStageMask                    = ui_wait_stages;

	const std::array<VkCommandBuffer, 1> ui_submit_command_buffers = {UI_command_buffer};

	ui_submit_info.commandBufferCount = static_cast<uint32_t>(ui_submit_command_buffers.size());
	ui_submit_info.pCommandBuffers    = ui_submit_command_buffers.data();

	const VkSemaphore ui_signal_semaphores[] = {UI_render_finished_semaphores->GetOne(currentFrame)};        //ui_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	ui_submit_info.signalSemaphoreCount      = 1;
	ui_submit_info.pSignalSemaphores         = ui_signal_semaphores;

	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &ui_submit_info, frame_fences->GetOne(currentFrame)))
	//**********************************************************************************************************************

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = ui_signal_semaphores;

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
		//TODO: recreation of swapchain
		//recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::DrawFrame()
{
	//*************************************************************************************
	//RenderGraph
	RenderGraph::DependencyGraph render_graph{renderpass_manager};        // you need to re-establish render graph for EVERY FRAME

	//*************************************************************************************
	//RESOURCES **CREATION** META INFO OF gbuffer position
	//这里的sampler和view是空的，因这两个东西是specific to a pass的，不同的pass也许会使用不同的sampler和view，也可以一开始就指定sampler和view（相当于指定默认的sampler和view）

	VkTexture::Descriptor G_buffer_position_dsctor{
	    .tex_img_PP  = TextureImgPP{DeferedRendering::G_position_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag},
	    .img_view_CI = std::nullopt,
	    .sample_CI   = std::nullopt,
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE,因为是每帧都需要资源，所以也许可以通过复用，消除这个bundle size存在的必要性！
	};

	//TODO:sampler 和 view资源管理

	//如果某个pass没有传入资源 要使用的sample_CI或者img_view_CI，那么就保持默认状态（不改变texture中的sampler和view指针）听起来不错，但是这样会不会让资源过度浪费？
	//传入一推创建参数，虚拟资源只有在execute执行的时候才会创建真正的资源
	G_buffer_position_dsctor.tex_img_PP->default_image_CI.usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	G_buffer_position_dsctor.tex_img_PP->default_image_CI.samples          = DeferedRendering::MSAA_sample_count;
	G_buffer_position_dsctor.tex_img_PP->default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //这个default_layout_right_aft_creation是不是可以取消了

	RenderGraph::Resource<VkTexture> G_buffer_position{G_buffer_position_dsctor};        //G_buffer_position代表这个资源，其中含有虚拟资源实例化时需要的信息，但是虚拟资源暂时还没有获得GPU资源

	//*************************************************************************************
	//RESOURCES **CREATION** META INFO OF gbuffer normal
	VkTexture::Descriptor G_buffer_normal_dsctor{
	    .tex_img_PP  = TextureImgPP{DeferedRendering::G_normal_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag},
	    .img_view_CI = std::nullopt,
	    .sample_CI   = std::nullopt,
	    .bundle_size = Vk::SWAP_IMG_COUNT};

	G_buffer_normal_dsctor.tex_img_PP->default_image_CI.usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	G_buffer_normal_dsctor.tex_img_PP->default_image_CI.samples          = DeferedRendering::MSAA_sample_count;
	G_buffer_normal_dsctor.tex_img_PP->default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        
	RenderGraph::Resource<VkTexture> G_buffer_normal{G_buffer_normal_dsctor};        

	//*************************************************************************************
	//RESOURCES **CREATION** META INFO OF gbuffer albedo
	VkTexture::Descriptor G_buffer_albedo_dsctor{
	    .tex_img_PP  = TextureImgPP{DeferedRendering::G_albedo_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag},
	    .img_view_CI = std::nullopt,
	    .sample_CI   = std::nullopt,
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE，很重要的信息or因为是每帧，所以也许可以通过复用，消除这个bundle size的必要性！
	};

	G_buffer_albedo_dsctor.tex_img_PP->default_image_CI.usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	G_buffer_albedo_dsctor.tex_img_PP->default_image_CI.samples          = DeferedRendering::MSAA_sample_count;
	G_buffer_albedo_dsctor.tex_img_PP->default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        

	RenderGraph::Resource<VkTexture> G_buffer_albedo{G_buffer_albedo_dsctor};        

	//*************************************************************************************
	//RESOURCES **CREATION** META INFO OF gbuffer posZGradient
	VkTexture::Descriptor G_buffer_posZGradient_dsctor{
	    .tex_img_PP  = TextureImgPP{DeferedRendering::G_posZgrad_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag},
	    .img_view_CI = std::nullopt,
	    .sample_CI   = std::nullopt,
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE，很重要的信息or因为是每帧，所以也许可以通过复用，消除这个bundle size的必要性！
	};

	G_buffer_posZGradient_dsctor.tex_img_PP->default_image_CI.usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	G_buffer_posZGradient_dsctor.tex_img_PP->default_image_CI.samples          = DeferedRendering::MSAA_sample_count;
	G_buffer_posZGradient_dsctor.tex_img_PP->default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image

	RenderGraph::Resource<VkTexture> G_buffer_posZGrad{G_buffer_posZGradient_dsctor};        
	                                                                                         //*************************************************************************************

	//*************************************************************************************
	//RESOURCES **CREATION** META INFO OF gbuffer depth
	VkTexture::Descriptor G_buffer_depth_dsctor{
	    .tex_img_PP  = TextureImgPP{DeferedRendering::G_depth_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag},
	    .img_view_CI = std::nullopt,
	    .sample_CI   = std::nullopt,
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE，很重要的信息or因为是每帧，所以也许可以通过复用，消除这个bundle size的必要性！
	};

	G_buffer_depth_dsctor.tex_img_PP->default_image_CI.usage            = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	G_buffer_depth_dsctor.tex_img_PP->default_image_CI.samples          = DeferedRendering::MSAA_sample_count;
	G_buffer_depth_dsctor.tex_img_PP->default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image

	RenderGraph::Resource<VkTexture> G_buffer_depth{G_buffer_depth_dsctor};        
	//*************************************************************************************


	//*************************************************************************************
	//PERSISTENT RESOURCES: Matirx uniform buffer
	RenderGraph::Resource<VkBufferBase> uniform_matrix_buffer{global_resources.matrix_buffer_gpu_defered_rendering[0]};        //G_buffer_position代表这个资源，其中含有虚拟资源创建需要的信息，但是暂时还没有获得GPU资源






	//*************************************************************************************
	//*************************************************************************************
	//TODO: META INFO of BACKBUFFER(or RESOURCES IMPORTED FROM OUTSIDE)

	const auto G_buffer_pos_slot = render_graph.AddResourceNode(G_buffer_position, "GBufferPosition");        //向渲染图中加入一个资源节点

	const auto G_buffer_normal_slot = render_graph.AddResourceNode(G_buffer_normal, "GBufferNormal");        //向渲染图中加入一个资源节点

	const auto G_buffer_albedo_slot = render_graph.AddResourceNode(G_buffer_albedo, "GBufferAlbedo");        //向渲染图中加入一个资源节点

	const auto G_buffer_posZGrad_slot = render_graph.AddResourceNode(G_buffer_posZGrad, "GBufferPosZGrad");        //向渲染图中加入一个资源节点

	const auto G_buffer_depth_slot = render_graph.AddResourceNode(G_buffer_depth, "GBufferDepth");        //向渲染图中加入一个资源节点

	const auto uniform_mat_buffer_slot = render_graph.AddResourceNode(uniform_matrix_buffer, "MatrixUB");        //向渲染图中加入一个资源节点




	auto DeferedGeoPass_handle = render_graph.AddGfxPassNode(renderpasses[0], "DeferedGeometryPass", {uniform_mat_buffer_slot}, {G_buffer_pos_slot, G_buffer_normal_slot, G_buffer_albedo_slot, G_buffer_posZGrad_slot, G_buffer_depth_slot});        //向渲染图中加入一个pass

	auto DeferedComPass_handle = render_graph.AddGfxPassNode(renderpasses[1], "DeferedCompositionPass", {G_buffer_pos_slot, G_buffer_normal_slot, G_buffer_albedo_slot, G_buffer_posZGrad_slot, G_buffer_depth_slot},{});        //向渲染图中加入一个pass



	//REGISTER RESOURCES USAGE WITHIN A PASS
	render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_buffer_pos_slot, std::make_unique<VkAttachmentInfo::WithinPassRG>(
	    Vk::RsrcInfoType::Attachment,
	    DeferedRendering::G_position_format,
	    Vk::AttachmentIndex<0>,
	    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
	    VkAttachmentInfo::Type::ColorAttachment,
	    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}
	
	    //TODO:sampler
	    //TODO:img view
	    //TODO:subresources range
	));



	render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_buffer_normal_slot, std::make_unique<VkAttachmentInfo::WithinPassRG>(
	    Vk::RsrcInfoType::Attachment,
	    DeferedRendering::G_normal_format,
	    Vk::AttachmentIndex<1>,
	    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
	    VkAttachmentInfo::Type::ColorAttachment,
	    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}
	  
	    //TODO:sampler
	    //TODO:img view
	    //TODO:subresources range
	));


	render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_buffer_albedo_slot, std::make_unique<VkAttachmentInfo::WithinPassRG>(
	    Vk::RsrcInfoType::Attachment,
	    DeferedRendering::G_albedo_format,
	    Vk::AttachmentIndex<2>,
	    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //these layout maybe not needed.

	    VkAttachmentInfo::Type::ColorAttachment,
	    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}

	    //TODO:sampler
	    //TODO:img view
	    //TODO:subresources range
	));


	render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_buffer_posZGrad_slot, std::make_unique<VkAttachmentInfo::WithinPassRG>(
			Vk::RsrcInfoType::Attachment,
			DeferedRendering::G_posZgrad_format,
			Vk::AttachmentIndex<3>,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //these layout maybe not needed.

			VkAttachmentInfo::Type::ColorAttachment,
			VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}

			//TODO:sampler
			//TODO:img view
			//TODO:subresources range
		));


	render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_buffer_depth_slot, std::make_unique<VkAttachmentInfo::WithinPassRG>(
			Vk::RsrcInfoType::Attachment,
			DeferedRendering::G_depth_format,
			Vk::AttachmentIndex<4>,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,        //

			VkAttachmentInfo::Type::DepthAttachment,
			VkClearValue{ .depthStencil{0.0f, 0}}

			//TODO:sampler
			//TODO:img view
			//TODO:subresources range
		));



	//REGISTER RESOURCES USAGE WITHIN A PASS
	render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, uniform_mat_buffer_slot,
		std::make_shared<VkBufferBase::WithinPassRG>(RsrcInfoType::UniformBuffer,Vk::Binding<0>,Vk::BindingArrayElement<0>,VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)

		//TODO:subresources range
	);


		 


	//RenderGraph编译
	if (render_graph.Compile())
	{
		constexpr size_t imageIndex{0};
		//RenderGraph执行
		bool successed = render_graph.Execute(command_manager.GetGraphicsCommandBuffers()[imageIndex]);
	}
	else
	{
		throw std::runtime_error("Failed to compile render graph!");
	}








}

void Renderer::UpdateCamera(float dt)
{
	//TODO:用命令模式优化
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

Renderer::Renderer(VkGraphicsComponent &gfx_) :
    BaseRenderer(gfx_), renderpass_manager(gfx), imgui_UI(gfx)
{
}

void Renderer::SetUpUserInput()
{
	std::vector<int> tracked_keys = {GLFW_KEY_ESCAPE, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_G, GLFW_KEY_UP, GLFW_KEY_DOWN};
	keyboard                      = std::make_unique<KeyBoardInputManager>(tracked_keys);
	keyboard->SetupKeyInputs(window.GetWindowPtr());

	mouse = std::make_unique<MouseInputManager>(gfx);
}

void Renderer::CreateCamera()
{
	camera = std::make_unique<FirstPersonCamera>();
	camera->SetFrustum(glm::radians(60.f), static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width) / static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height), 0.1f, 256.f);
}

void Renderer::CreateCommomAttachmentImgs()
{
	CreateSwapchainTextures();
	CreateDepthTextures();
}
//

void Renderer::CreateCommonTextureImgs()
{
	//const auto &      texture_factory       = renderpass_manager.GetTextureFactory();
	//const std::string chiricahua_narrowPath_path = "../../data/textures/hdr/NarrowPath_3k.hdr";
	//const VkTextureFactory::SamplerPP sampler_ppl;
	//global_resources.hdr_environment_map = texture_factory.ProduceTexture(chiricahua_narrowPath_path,VK_FORMAT_R32G32B32A32_SFLOAT,sampler_ppl);
}

//the creation of this resources should be taken care of by render graph, which haven't been implemented.
void Renderer::CreateDepthTextures()
{
	//Use depth stencil format, cuz we will do stencil writing.
	const DepthImgPP depth_img_PP{gfx};
	auto             img_view_CI       = ImgViewCI::PopulateDepthImgViewCI(gfx.SwapchainMan());
	global_resources.depth_attachments = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(depth_img_PP, std::nullopt, img_view_CI, SWAP_IMG_COUNT);
}

//the creation of this resources should be taken care of by render graph, which haven't been implemented.
void Renderer::CreateSwapchainTextures()
{
	const SwapchainImgPP swap_img_PP;
	auto                 img_view_CI       = ImgViewCI::PopulateSwapchainImgViewCI(gfx.SwapchainMan());
	global_resources.swapchain_attachments = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(swap_img_PP, std::nullopt, img_view_CI);
}
