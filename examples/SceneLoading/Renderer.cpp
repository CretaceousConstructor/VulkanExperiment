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
	persistent_resources.sponza   = model_factory.GetGltfModel<NonPbrMaterial>(sponza_path, Vk::ModelLoadingOption::BindlessRenderingMode, 0);

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
		persistent_resources.matrix_buffer_gpu_defered_rendering = buffer_factory.ProduceBufferPtrArray(sizeof(DeferedRendering::UBO), Vk::BundleSize<SWAP_IMG_COUNT>, unim_buf_PP);
	}
}

void Renderer::CreateCommonDescriptorPool()
{
	// Create the global descriptor pool
	//TODO:����ķ�����Ŀ

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

	const auto desc_pool_CI   = Vk::GetDescriptorPoolCI(desc_pool_sizes, Vk::SetCount<SWAP_IMG_COUNT>);
	persistent_resources.pool = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
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

	//renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<DeferedGeometryPass>(gfx, renderpass_manager, persistent_resources));
	//renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<DeferedCompositionPass>(gfx, renderpass_manager, persistent_resources));

	//renderpasses.push_back(VkRenderpassManager::ProduceRenderpass<MSAAPass>(gfx, renderpass_manager, global_resources));

	renderpasses.push_back(std::make_shared<DeferedGeometryPass>(gfx, renderpass_manager, persistent_resources));
	renderpasses.push_back(std::make_shared<DeferedCompositionPass>(gfx, renderpass_manager, persistent_resources));
}

void Renderer::InitSynObjects()
{
	const auto &syn_obj_factory{renderpass_manager.GetSynOjectFactory()};

	image_available_semaphores            = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	general_rendering_finished_semaphores = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	UI_rendering_finished_semaphores      = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	frame_fences                          = syn_obj_factory.GetFenceBundle(MAX_FRAMES_IN_FLIGHT, VkSynObjectBundleBase::SyncObjCreateOption::Signaled);
	image_fences.resize(swapchain_manager.GetSwapImageCount());
}

void Renderer::UpdateUniformBuffer(size_t current_image_index)
{
	//struct UBO_VS_SCENE{
	//	glm::mat4 projection;
	//	glm::mat4 view;
	//	glm::vec4 light_pos   = glm::vec4(0.0f, 2.5f, 0.0f, 1.0f);
	//	glm::vec4 view_pos;
	//} ubo_vs_scene;                   //���ڶ�����ɫ����uniform buffer object

	////FLLOWING SHIT IS USED BY PBR
	//global_resources.matrix_buffer_cpu.projection = camera->GetProjMatrix();
	//global_resources.matrix_buffer_cpu.view = camera->GetViewMatrix();
	//global_resources.matrix_buffer_cpu.cam_pos = camera->GetEyePos();
	//global_resources.matrix_buffer_gpu[current_image_index]->CopyFrom(&global_resources.matrix_buffer_cpu, sizeof(global_resources.matrix_buffer_cpu));

	static bool first_time = true;
	if (first_time)
	{
		/*******************************cpu side ubuffer init**********************************/
		// Lights
		// White
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[0].position = glm::vec4(1.0f, .7f, 0.f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[0].color    = glm::vec3(1.5f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[0].radius   = 15.0f * 0.25f;
		// Red
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[1].position = glm::vec4(-1.0f, .7f, 0.f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[1].color    = glm::vec3(1.0f, 0.0f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[1].radius   = 15.0f;
		// Blue
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[2].position = glm::vec4(5.5f, .7f, 0.f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[2].color    = glm::vec3(0.0f, 0.0f, 2.5f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[2].radius   = 5.0f;
		// Yellow
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[3].position = glm::vec4(-5.5f, .7f, 0.f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[3].color    = glm::vec3(1.0f, 1.0f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[3].radius   = 2.0f;
		// Green
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[4].position = glm::vec4(9.5f, .7f, 0.f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[4].color    = glm::vec3(0.0f, 1.0f, 0.2f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[4].radius   = 5.0f;
		// Yellow
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[5].position = glm::vec4(-9.5f, .7f, 0.f, 0.0f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[5].color    = glm::vec3(1.0f, 0.7f, 0.3f);
		persistent_resources.matrix_buffer_cpu_defered_rendering.lights[5].radius   = 25.0f;
	}

	persistent_resources.matrix_buffer_cpu_defered_rendering.projection   = camera->GetProjMatrix(Camera::ProjectionMtxSetting::ReversedZ);        //for revered Z
	persistent_resources.matrix_buffer_cpu_defered_rendering.view         = camera->GetViewMatrix();
	persistent_resources.matrix_buffer_cpu_defered_rendering.view_inverse = camera->GetInverseViewMatrix();
	persistent_resources.matrix_buffer_cpu_defered_rendering.cam_pos      = camera->GetEyePos();

	//COPY
	persistent_resources.matrix_buffer_gpu_defered_rendering[current_image_index]->CopyFrom(&persistent_resources.matrix_buffer_cpu_defered_rendering, sizeof(persistent_resources.matrix_buffer_cpu_defered_rendering));

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
	//TODO:imageʹ����Ϻ�subpass dependency�Ĺ�ϵ
	static size_t currentFrame = 0;
	////����fence    ��Ĭ�ϳ�ʼ������ָ����ʼ״̬��ʱ������unsignaled��״̬
	////����semaphore��Ĭ�ϳ�ʼ������ָ����ʼ״̬��ʱ������unsignaled��״̬
	///
	///
	////�ȴ�frame
	vkWaitForFences(device_manager.GetLogicalDevice(), 1, &(frame_fences->GetOne(currentFrame)), VK_TRUE, UINT64_MAX);        //vkWaitForFences����ʱ����CPU���ȴ�fence��signal�� �� unsignaled״̬ ��� signaled״̬�Ż�ֹͣ������                  To wait for one or more fences to enter the signaled state on the host,

	uint32_t imageIndex;

	//��õ���һ��image ��index�󣬿������index��Ӧ��image��û�б�presentation engineʹ����ϣ�������Ҫһ��image_available_semaphores->GetOne(currentFrame)��   ֪ͨ��һ���GPU   �Ѿ����������index��Ӧimage����Ⱦ�ˡ�
	VkResult result = vkAcquireNextImageKHR(device_manager.GetLogicalDevice(), swapchain_manager.GetSwapChain(), UINT64_MAX, image_available_semaphores->GetOne(currentFrame), nullptr, &imageIndex);        //As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.�õ���һ������ʹ�õ�image��index���������image���ܻ�û���꣬�����õ�imageIndex��Ӧ��image���п���������̵�ʱ���ڱ�ĳһ֡ʹ����ϵ���һ������vulkanʵ�־������

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

	//images_fence��СΪ3����������ע�����Ĵ�С����2(MAX_FRAMES_IN_FLIGHT)
	//��ֹimage���ڱ�GPUʹ��
	if (image_fences[imageIndex] != nullptr)
	{
		//	images_inflight[imageIndex] ���� nullptr��˵��ĳһ֡����ռ��GPU��Դ(��image�йص���Դ)��ЩGPU��Դ���ڱ���imageIndexΪ�±��imageʹ�ã���ô���Ǿ�Ҫ�ȴ���
		//TODO:Ŀǰ�������ÿһ��image����һ����Դ������� max_frame_in_flightС��swap chain image�Ļ���ֻ��Ҫmax_frame_in_flight����Դ�Ϳ����ˣ����������ô������Ҫ��¼��������һ֡��ʹ��images_inflight[imageIndex]��������Դ���Ӷ��������WaitForFences����Ժ�Ϳ���ʹ�ÿճ�������Դ
		//	���޵ȴ�fence,
		vkWaitForFences(device_manager.GetLogicalDevice(), 1, &image_fences[imageIndex], VK_TRUE, UINT64_MAX);
	}

	image_fences[imageIndex] = frame_fences->GetOne(currentFrame);        //�ȴ���images�Ϻ��ý������ᱻGPU������ɫ��image��currentFrame��ռ��(��ʾcurrentFrame��һ֡��GPU��Դ���ڱ�indexΪimageIndex��imageռ��)��Ŀǰinflight_fences[currentFrame]����signled��״̬��

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
	//��Ϊ������image_fences[imageIndex] = frame_fences[currentFrame];  ������ʱ��image_fences[imageIndex]�� frame_fences[currentFrame]����ͬ����״̬;����Ӧ��ͬʱΪunsignaled״̬

	//frame_fences[currentFrame]; ��ʱ��image_fences[imageIndex]�� frame_fences[currentFrame]����ͬ����״̬;����Ӧ��ͬʱΪunsignaled״̬����ôresetΪunsignal״̬��ʾ������Ҫռ���ˣ�
	vkResetFences(device_manager.GetLogicalDevice(), 1, &frame_fences->GetOne(currentFrame));        //To set the state of fences to unsignaled from the host side
	//vkQueueSubmit:   fence(last parameter is an optional handle to a fence to be signaled once all submitted command buffers have completed execution. If fence is not VK_NULL_HANDLE, it defines a fence signal operation.��command buffer�е�����ִ�н����Ժ�Ҳ����GPU��Ⱦ����Ժ�

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
	const VkSemaphore              gfx_wait_semaphores[] = {image_available_semaphores->GetOne(currentFrame)};        //�����ǵȴ�imageIndex��Ӧ��image����ռ�õ�GPU��Դ�ͷų�������������Ϊimage��Ⱦ��ɺ󣬼���GPU��Դ�ͷ��ˣ����ǻ���Ҫ����presentation�����п���presentation engine��û����ɣ�������Ҫ�ȴ����semaphore
	constexpr VkPipelineStageFlags gfx_wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	gfx_submit_info.waitSemaphoreCount                   = 1;
	gfx_submit_info.pWaitSemaphores                      = gfx_wait_semaphores;
	gfx_submit_info.pWaitDstStageMask                    = gfx_wait_stages;

	const auto graphics_command_buffer = command_manager.GetGraphicsCommandBuffers()[imageIndex];

	const std::array<VkCommandBuffer, 1> submit_command_buffers = {graphics_command_buffer};

	gfx_submit_info.commandBufferCount = static_cast<uint32_t>(submit_command_buffers.size());
	//&graphics_command_buffers[imageIndex] �õ�frame buffer����swap image[imageIndex]
	gfx_submit_info.pCommandBuffers = submit_command_buffers.data();

	const VkSemaphore gfx_signal_semaphores[] = {general_rendering_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffersִ�����Ժ��signal������ui_command_buffer��֪�����Կ�ʼ��ȾUI�ˡ�
	gfx_submit_info.signalSemaphoreCount      = 1;
	gfx_submit_info.pSignalSemaphores         = gfx_signal_semaphores;

	//VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &gfx_submit_info, frame_fences->GetOne(currentFrame)))
	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &gfx_submit_info, VK_NULL_HANDLE))
	//**********************************************************************************************************************
	//UI Rendering command buffer submitting.
	VkSubmitInfo ui_submit_info{};
	const auto   UI_command_buffer                      = imgui_UI.GetCommandBuffer(imageIndex);
	ui_submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	const VkSemaphore              ui_wait_semaphores[] = {general_rendering_finished_semaphores->GetOne(currentFrame)};
	constexpr VkPipelineStageFlags ui_wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	ui_submit_info.waitSemaphoreCount                   = 1;
	ui_submit_info.pWaitSemaphores                      = ui_wait_semaphores;
	ui_submit_info.pWaitDstStageMask                    = ui_wait_stages;

	const std::array<VkCommandBuffer, 1> ui_submit_command_buffers = {UI_command_buffer};

	ui_submit_info.commandBufferCount = static_cast<uint32_t>(ui_submit_command_buffers.size());
	ui_submit_info.pCommandBuffers    = ui_submit_command_buffers.data();

	const VkSemaphore ui_signal_semaphores[] = {UI_rendering_finished_semaphores->GetOne(currentFrame)};        //ui_command_buffersִ�����Ժ��signal������presentation engine֪����Ⱦ��ɿ���չʾ�ˡ�
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

void Renderer::DrawFrame(float time_diff, int)
{
	static size_t currentFrame = 0;
	////����fence    ��Ĭ�ϳ�ʼ������ָ����ʼ״̬��ʱ������unsignaled��״̬
	////����semaphore��Ĭ�ϳ�ʼ������ָ����ʼ״̬��ʱ������unsignaled��״̬
	///
	///
	////�ȴ�frame
	vkWaitForFences(device_manager.GetLogicalDevice(), 1, &(frame_fences->GetOne(currentFrame)), VK_TRUE, UINT64_MAX);        //vkWaitForFences����ʱ����CPU���ȴ�fence��signal�� �� unsignaled״̬ ��� signaled״̬�Ż�ֹͣ������                  To wait for one or more fences to enter the signaled state on the host,

	uint32_t img_index;

	//��õ���һ��image ��index�󣬿������index��Ӧ��image��û�б�presentation engineʹ����ϣ�������Ҫһ��image_available_semaphores->GetOne(currentFrame)��   ֪ͨ��һ���GPU   �Ѿ����������index��Ӧimage����Ⱦ�ˡ�
	VkResult result = vkAcquireNextImageKHR(device_manager.GetLogicalDevice(), swapchain_manager.GetSwapChain(), UINT64_MAX, image_available_semaphores->GetOne(currentFrame), nullptr, &img_index);        //As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.�õ���һ������ʹ�õ�image��index���������image���ܻ�û���꣬�����õ�imageIndex��Ӧ��image���п���������̵�ʱ���ڱ�ĳһ֡ʹ����ϵ���һ������vulkanʵ�־������

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

	UpdateUniformBuffer(img_index);

	//images_fence��СΪ3����������ע�����Ĵ�С����2(MAX_FRAMES_IN_FLIGHT��2)
	//��ֹimage���ڱ�GPUʹ��
	if (nullptr != image_fences[img_index])        //��ֹ��һֱ֡�ӱ���
	{
		//	images_inflight[imageIndex] ���� nullptr��˵��ĳһ֡����ռ��GPU��Դ(��image�йص���Դ)��ЩGPU��Դ���ڱ���imageIndexΪ�±��imageʹ�ã���ô���Ǿ�Ҫ�ȴ���
		//TODO:Ŀǰ�������ÿһ��image����һ����Դ������� max_frame_in_flightС��swap chain image�Ļ���ֻ��Ҫmax_frame_in_flight����Դ�Ϳ����ˣ����������ô������Ҫ��¼��������һ֡��ʹ��images_inflight[imageIndex]��������Դ���Ӷ��������WaitForFences����Ժ�Ϳ���ʹ�ÿճ�������Դ
		//	���޵ȴ�fence,
		vkWaitForFences(device_manager.GetLogicalDevice(), 1, &image_fences[img_index], VK_TRUE, UINT64_MAX);
	}

	image_fences[img_index] = frame_fences->GetOne(currentFrame);        //�ȴ���images�Ϻ��ý������ᱻGPU������ɫ��image��currentFrame��ռ��(��ʾcurrentFrame��һ֡��GPU��Դ���ڱ�indexΪimageIndex��imageռ��)��Ŀǰinflight_fences[currentFrame]����signled��״̬��

	////���Կ�������graphics_command_buffers[imageIndex]ʹ�����Ժ�inflight_fences_frame[currentFrame]�ᱻsignal����vkQueueSubmit�Ķ��壩����ô��ôȷ�������graphics_command_buffers[imageIndex]�Ѿ���ǰ��֡ʹ��������أ�

	//��Ϊ������image_fences[imageIndex] = frame_fences[currentFrame];  ������ʱ��image_fences[imageIndex]�� frame_fences[currentFrame]����ͬ����״̬;����Ӧ��ͬʱΪunsignaled״̬

	//frame_fences[currentFrame]; ��ʱ��image_fences[imageIndex]�� frame_fences[currentFrame]����ͬ����״̬;����Ӧ��ͬʱΪunsignaled״̬����ôresetΪunsignal״̬��ʾ������Ҫռ���ˣ�
	vkResetFences(device_manager.GetLogicalDevice(), 1, &frame_fences->GetOne(currentFrame));        //To set the state of fences to unsignaled from the host side
	//vkQueueSubmit:   fence(last parameter is an optional handle to a fence to be signaled once all submitted command buffers have completed execution. If fence is not VK_NULL_HANDLE, it defines a fence signal operation.��command buffer�е�����ִ�н����Ժ�Ҳ����GPU��Ⱦ����Ժ�

	//UI Reset command buffers
	imgui_UI.ResetCommandPoolAndBeginCommandBuffer(img_index);

	//UI Describing UI
	imgui_UI.DescribingUI();
	imgui_UI.TestDescribingUI(camera->GetEyePos());

	//UI command buffer recording
	imgui_UI.RenderingCommandRecord(img_index);

	UpdateCamera(time_diff);
	//**********************************************************************************************************************
	//Graphics Rendering command buffer submitting.
	VkSubmitInfo gfx_submit_info{};
	gfx_submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	const VkSemaphore              gfx_wait_semaphores[] = {image_available_semaphores->GetOne(currentFrame)};        //�����ǵȴ�imageIndex��Ӧ��image����ռ�õ�GPU��Դ�ͷų�������������ΪҪ�ȴ�image��Ⱦ��ɡ�����imageռ�õ�GPU��Դ�ͷ��ˣ����ǻ���Ҫ����presentation�����п���presentation engine��û����ɣ�������Ҫ�ȴ����semaphore
	constexpr VkPipelineStageFlags gfx_wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	gfx_submit_info.waitSemaphoreCount                   = 1;
	gfx_submit_info.pWaitSemaphores                      = gfx_wait_semaphores;
	gfx_submit_info.pWaitDstStageMask                    = gfx_wait_stages;

	const auto graphics_command_buffer = command_manager.GetGraphicsCommandBuffers()[img_index];

	CommandBufferRecording(graphics_command_buffer, img_index);

	const std::array<VkCommandBuffer, 1> submit_command_buffers = {graphics_command_buffer};

	gfx_submit_info.commandBufferCount = static_cast<uint32_t>(submit_command_buffers.size());
	//&graphics_command_buffers[imageIndex] �õ�frame buffer����swap image[imageIndex]
	gfx_submit_info.pCommandBuffers = submit_command_buffers.data();

	const VkSemaphore gfx_signal_semaphores[] = {general_rendering_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffersִ�����Ժ��signal������ui_command_buffer��֪�����Կ�ʼ��ȾUI�ˡ�
	gfx_submit_info.signalSemaphoreCount      = 1;
	gfx_submit_info.pSignalSemaphores         = gfx_signal_semaphores;

	//VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &gfx_submit_info, frame_fences->GetOne(currentFrame)))
	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &gfx_submit_info, VK_NULL_HANDLE))
	//**********************************************************************************************************************
	//UI Rendering command buffer submitting.
	VkSubmitInfo ui_submit_info{};
	const auto   UI_command_buffer                      = imgui_UI.GetCommandBuffer(img_index);
	ui_submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	const VkSemaphore              ui_wait_semaphores[] = {general_rendering_finished_semaphores->GetOne(currentFrame)};
	constexpr VkPipelineStageFlags ui_wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	ui_submit_info.waitSemaphoreCount                   = 1;
	ui_submit_info.pWaitSemaphores                      = ui_wait_semaphores;
	ui_submit_info.pWaitDstStageMask                    = ui_wait_stages;

	const std::array<VkCommandBuffer, 1> ui_submit_command_buffers = {UI_command_buffer};

	ui_submit_info.commandBufferCount = static_cast<uint32_t>(ui_submit_command_buffers.size());
	ui_submit_info.pCommandBuffers    = ui_submit_command_buffers.data();

	const VkSemaphore ui_signal_semaphores[] = {UI_rendering_finished_semaphores->GetOne(currentFrame)};        //ui_command_buffersִ�����Ժ��signal������presentation engine֪����Ⱦ��ɿ���չʾ�ˡ�
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
	presentInfo.pImageIndices         = &img_index;
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

void Renderer::UpdateCamera(float dt)
{
	//TODO:������ģʽ�Ż�
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
	auto             img_view_CI           = ImgViewCI::PopulateDepthImgViewCI(gfx.SwapchainMan());
	persistent_resources.depth_attachments = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(depth_img_PP, std::nullopt, img_view_CI, SWAP_IMG_COUNT);
}

//the creation of this resources should be taken care of by render graph, which haven't been implemented.
void Renderer::CreateSwapchainTextures()
{
	const SwapchainImgPP swap_img_PP;
	auto                 img_view_CI           = ImgViewCI::PopulateSwapchainImgViewCI(gfx.SwapchainMan());
	persistent_resources.swapchain_attachments = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(swap_img_PP, std::nullopt, img_view_CI);
}

void Renderer::CommandBufferRecording(VkCommandBuffer cmd_buf, size_t img_index)
{
	VkCommandManager::BeginCommandBuffers(cmd_buf);

	//*************************************************************************************
	//RENDERGRAPH
	RenderGraph::DependencyGraph render_graph{renderpass_manager};        // you need to re-establish render graph for EVERY FRAME

	//TRANSIENT RESOURCES **CREATION** META INFO
	//*************************************************************************************
	//RESOURCES **CREATION** META INFO OF gbuffer position
	//�����sampler��view�ǿյģ���������������specific to a pass�ģ���ͬ��passҲ���ʹ�ò�ͬ��sampler��view��Ҳ����һ��ʼ��ָ��sampler��view���൱��ָ��Ĭ�ϵ�sampler��view��

	VkTexture::Descriptor G_buffer_position_dsctor{
	    .tex_img_PP  = TextureImgPP{DeferedRendering::G_position_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag},
	    .img_view_CI = std::nullopt,
	    .sample_CI   = std::nullopt,
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE,��Ϊ��ÿ֡����Ҫ��Դ������Ҳ�����ͨ�����ã��������bundle size���ڵı�Ҫ�ԣ�
	};

	//TODO:sampler �� view��Դ����

	//���ĳ��passû�д�����Դ Ҫʹ�õ�sample_CI����img_view_CI����ô�ͱ���Ĭ��״̬�����ı�texture�е�sampler��viewָ�룩�������������������᲻������Դ�����˷ѣ�
	//����һ�ƴ���������������Դֻ����executeִ�е�ʱ��Żᴴ����������Դ
	G_buffer_position_dsctor.tex_img_PP->default_image_CI.usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	G_buffer_position_dsctor.tex_img_PP->default_image_CI.samples          = DeferedRendering::MSAA_sample_count;
	G_buffer_position_dsctor.tex_img_PP->default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //���default_layout_right_aft_creation�ǲ��ǿ���ȡ����

	RenderGraph::Resource<VkTexture> G_buffer_position{G_buffer_position_dsctor};        //G_buffer_position���������Դ�����к���������Դʵ����ʱ��Ҫ����Ϣ������������Դ��ʱ��û�л��GPU��Դ

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
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE������Ҫ����Ϣor��Ϊ��ÿ֡������Ҳ�����ͨ�����ã��������bundle size�ı�Ҫ�ԣ�
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
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE������Ҫ����Ϣor��Ϊ��ÿ֡������Ҳ�����ͨ�����ã��������bundle size�ı�Ҫ�ԣ�
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
	    .bundle_size = Vk::SWAP_IMG_COUNT        //BUNDLE SIZE������Ҫ����Ϣor��Ϊ��ÿ֡������Ҳ�����ͨ�����ã��������bundle size�ı�Ҫ�ԣ�
	};

	G_buffer_depth_dsctor.tex_img_PP->default_image_CI.usage            = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	G_buffer_depth_dsctor.tex_img_PP->default_image_CI.samples          = DeferedRendering::MSAA_sample_count;
	G_buffer_depth_dsctor.tex_img_PP->default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image

	RenderGraph::Resource<VkTexture> G_buffer_depth{G_buffer_depth_dsctor};
	//RenderGraph::Resource  G_buffer_depth{G_buffer_depth_dsctor};
	//*************************************************************************************

	//*************************************************************************************
	//PERSISTENT RESOURCES
	//Matirx Uniform buffer
	RenderGraph::Resource uniform_matrix_buffer{persistent_resources.matrix_buffer_gpu_defered_rendering[img_index]};
	//Swap Chain Color Attachment
	RenderGraph::Resource swap_color_attach{persistent_resources.swapchain_attachments[img_index]};
	//Swap Chain Depth Attachment
	RenderGraph::Resource swap_depth_attach{persistent_resources.depth_attachments[img_index]};
	//*************************************************************************************
	//*************************************************************************************
	//TODO: META INFO of BACKBUFFER(or RESOURCES IMPORTED FROM OUTSIDE)

	const auto G_pos_slot = render_graph.AddResourceNode(G_buffer_position, "GBufferPosition");        //����Ⱦͼ�м���һ����Դ�ڵ�

	const auto G_normal_slot = render_graph.AddResourceNode(G_buffer_normal, "GBufferNormal");        //����Ⱦͼ�м���һ����Դ�ڵ�

	const auto G_albedo_slot = render_graph.AddResourceNode(G_buffer_albedo, "GBufferAlbedo");        //����Ⱦͼ�м���һ����Դ�ڵ�

	const auto G_posZGrad_slot = render_graph.AddResourceNode(G_buffer_posZGrad, "GBufferPosZGrad");        //����Ⱦͼ�м���һ����Դ�ڵ�

	const auto G_depth_slot = render_graph.AddResourceNode(G_buffer_depth, "GBufferDepth");        //����Ⱦͼ�м���һ����Դ�ڵ�

	const auto uniform_buffer_mat_slot = render_graph.AddResourceNode(uniform_matrix_buffer, "MatrixUB");        //����Ⱦͼ�м���һ����Դ�ڵ�

	const auto swap_color_slot = render_graph.AddResourceNode(swap_color_attach, "MatrixUB");        //����Ⱦͼ�м���һ����Դ�ڵ�

	const auto swap_depth_slot = render_graph.AddResourceNode(swap_depth_attach, "MatrixUB");        //����Ⱦͼ�м���һ����Դ�ڵ�

	auto DeferedGeoPass_handle = render_graph.AddGfxPassNode(renderpasses[0], "DeferedGeometryPass",
	                                                         {uniform_buffer_mat_slot},
	                                                         {G_pos_slot, G_normal_slot, G_albedo_slot, G_posZGrad_slot, G_depth_slot});        //����Ⱦͼ�м���һ��pass

	auto DeferedComPass_handle = render_graph.AddGfxPassNode(renderpasses[1], "DeferedCompositionPass",
	                                                         {uniform_buffer_mat_slot, G_pos_slot, G_normal_slot, G_albedo_slot, G_posZGrad_slot, G_depth_slot},
	                                                         {swap_color_slot, swap_depth_slot});        //����Ⱦͼ�м���һ��pass

	//Defered Geo Pass Resources Usage Recording
	{
		//REGISTER RESOURCES USAGE WITHIN A PASS
		render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_pos_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
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

		render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_normal_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Attachment,
		                                    DeferedRendering::G_normal_format,
		                                    Vk::AttachmentIndex<1>,
		                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::ColorAttachment,
		                                    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}));

		render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_albedo_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Attachment,
		                                    DeferedRendering::G_albedo_format,
		                                    Vk::AttachmentIndex<2>,
		                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::ColorAttachment,
		                                    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}));

		render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_posZGrad_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Attachment,
		                                    DeferedRendering::G_posZgrad_format,
		                                    Vk::AttachmentIndex<3>,
		                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::ColorAttachment,
		                                    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}));

		render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, G_depth_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Attachment,
		                                    DeferedRendering::G_depth_format,
		                                    Vk::AttachmentIndex<4>,
		                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		                                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::DepthAttachment,
		                                    VkClearValue{.depthStencil{0.0f, 0}}));

		//REGISTER RESOURCES USAGE WITHIN A PASS
		render_graph.RegisterRsrcsUsage(DeferedGeoPass_handle, uniform_buffer_mat_slot,
		                                std::make_unique<VkBufferBase::WithinPassRG>(
		                                    RsrcInfoType::UniformBuffer,
		                                    Vk::Binding<0>, Vk::BindingArrayElement<0>,
		                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT));

		//TODO:subresources range
	}




	//Defered Composition Pass Resources Usage Recording
	{
		//REGISTER RESOURCES USAGE WITHIN A PASS
		render_graph.RegisterRsrcsUsage(DeferedComPass_handle, G_pos_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Texture,
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

		render_graph.RegisterRsrcsUsage(DeferedComPass_handle, G_normal_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Texture,
		                                    DeferedRendering::G_normal_format,
		                                    Vk::AttachmentIndex<1>,
		                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::ColorAttachment,
		                                    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}));

		render_graph.RegisterRsrcsUsage(DeferedComPass_handle, G_albedo_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Texture,
		                                    DeferedRendering::G_albedo_format,
		                                    Vk::AttachmentIndex<2>,
		                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::ColorAttachment,
		                                    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}));

		render_graph.RegisterRsrcsUsage(DeferedComPass_handle, G_posZGrad_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Texture,
		                                    DeferedRendering::G_posZgrad_format,
		                                    Vk::AttachmentIndex<3>,
		                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::ColorAttachment,
		                                    VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}}));

		render_graph.RegisterRsrcsUsage(DeferedComPass_handle, G_depth_slot,
		                                std::make_unique<VkAttachmentInfo::WithinPassRG>(
		                                    Vk::RsrcInfoType::Texture,
		                                    DeferedRendering::G_depth_format,
		                                    Vk::AttachmentIndex<4>,
		                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		                                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,        //
		                                    VkAttachmentInfo::Type::DepthAttachment,
		                                    VkClearValue{.depthStencil{0.0f, 0}}));

		//REGISTER RESOURCES USAGE WITHIN A PASS
		render_graph.RegisterRsrcsUsage(DeferedComPass_handle, uniform_buffer_mat_slot,
		                                std::make_unique<VkBufferBase::WithinPassRG>(
		                                    RsrcInfoType::UniformBuffer,
		                                    Vk::Binding<0>, Vk::BindingArrayElement<0>,
		                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		                                    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT));
	}





	//RenderGraph����
	if (render_graph.Compile())
	{
		//RenderGraphִ��
		bool successed = render_graph.Execute(cmd_buf);

		if (!successed)
		{
			throw std::runtime_error("Failed to compile render graph!");
		}
	}
	else
	{
		throw std::runtime_error("Failed to compile render graph!");
	}

	VkCommandManager::EndCommandBuffers(cmd_buf);
}
