#define LIGHTCOUNT 6
#define MSAA_SAMPLE_COUNT 4

struct VSOutput
{
    float4 posViewport : SV_Position; // SV for system value
    [[vk::location(0)]] float2 uv : TEXCOORD0;
};


struct Light
{
    float4 position;
    float3 color;
    float radius;
};



struct UBOScene
{
    float4x4 projection;
    float4x4 view;
    float4x4 view_inverse;
    Light lights[LIGHTCOUNT];
    float3 cam_pos;
};



//[[vk::binding(X[, Y])]] can be attached to global variables to specify the descriptor set as Y and binding number as X.

//set 0,binding 0
//[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;


////set 0,binding 1
//[[vk::combinedImageSampler]][[vk::binding(1,0)]]
//Texture2DMS<float4, MSAA_SAMPLE_COUNT> GPosition; //MS for multisampled
////[[vk::combinedImageSampler]][[vk::binding(1,0)]]
////SamplerState samplerGPosition;

////set 0,binding 2
//[[vk::combinedImageSampler]][[vk::binding(2,0)]]
//Texture2DMS<float4, MSAA_SAMPLE_COUNT> GNormal;
////[[vk::combinedImageSampler]][[vk::binding(2,0)]]
////SamplerState samplerGNormal;


////set 0,binding 3
//[[vk::combinedImageSampler]][[vk::binding(3,0)]]
//Texture2DMS<float4, MSAA_SAMPLE_COUNT> GAlbedo;
////[[vk::combinedImageSampler]][[vk::binding(3,0)]]
////SamplerState samplerGAlbedo;


////set 0,binding 4 
//[[vk::combinedImageSampler]][[vk::binding(4,0)]]
//Texture2DMS<float2, MSAA_SAMPLE_COUNT> GposZGrad;
////[[vk::combinedImageSampler]][[vk::binding(4,0)]]
////SamplerState samplerGposZGrad;

////set 0,binding 5
//[[vk::combinedImageSampler]][[vk::binding(5,0)]]
//Texture2DMS<float, MSAA_SAMPLE_COUNT> GDepth;
////[[vk::combinedImageSampler]][[vk::binding(5,0)]]
////SamplerState samplerGDepth;
///set 0,binding 0
[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;


//set 0,binding 1
//[[vk::combinedImageSampler]][[vk::binding(1,0)]]
[[vk::binding(1,0)]]
//Texture2DMS<float4, MSAA_SAMPLE_COUNT> GPosition; //MS for multisampled
//[[vk::combinedImageSampler]][[vk::binding(1,0)]]
//SamplerState samplerGPosition;

Texture2DMS<float4, MSAA_SAMPLE_COUNT> GPosition; //MS for multisampled



//set 0,binding 2
//[[vk::combinedImageSampler]][[vk::binding(2,0)]]
[[vk::binding(2,0)]]
Texture2DMS<float4, MSAA_SAMPLE_COUNT> GNormal;
//[[vk::combinedImageSampler]][[vk::binding(2,0)]]
//SamplerState samplerGNormal;


//set 0,binding 3
//[[vk::combinedImageSampler]][[vk::binding(3,0)]]
[[vk::binding(3,0)]]
Texture2DMS<float4, MSAA_SAMPLE_COUNT> GAlbedo;
//[[vk::combinedImageSampler]][[vk::binding(3,0)]]
//SamplerState samplerGAlbedo;


//set 0,binding 4 
//[[vk::combinedImageSampler]][[vk::binding(4,0)]]
[[vk::binding(4,0)]]
Texture2DMS<float2, MSAA_SAMPLE_COUNT> GposZGrad;
//[[vk::combinedImageSampler]][[vk::binding(4,0)]]
//SamplerState samplerGposZGrad;

//set 0,binding 5
//[[vk::combinedImageSampler]][[vk::binding(5,0)]]
[[vk::binding(5,0)]]
Texture2DMS<float, MSAA_SAMPLE_COUNT> GDepth;
//[[vk::combinedImageSampler]][[vk::binding(5,0)]]
//SamplerState samplerGDepth;





struct SurfaceData
{
    float4 albedo;
    float3 pos;
    float3 pos_DX;
    float3 pos_DY;
    float3 normal;
};


float3 ComputeNDCCoordFromFramebufferZcoord(const float2 current_RT_dim, const float2 g_buffer_dim, const uint2 frag_coord, const float zf)
{

    //������ӿ���任�õ���gbuffer��Ⱦʱ���ӿڡ������current render target���ӿ�ʵ����Ҳ���ԣ��Ͳ����� ��ǰ��framebuffer���굽��Ⱦgbufferʱ��framebuffer����ı任��
	//�����ӿ���任

	//gbuffer��Ⱦʱ��Ĭ���ӿڣ���������ȾG-bufferʱʹ�õ�view port��G-bufferά��һ���Ļ�����ô�����extend_of_gbuffer��g_buffer_dim��һ��
	//VkViewport       viewport{};
	//const VkExtent3D extend_of_gbuffer = swapchain_manager.GetSwapChainImageExtent();
	//viewport.x                            = 0.0f;
	//viewport.y                            = static_cast<float>(extend_of_gbuffer.height);
	//viewport.width                        = static_cast<float>(extend_of_gbuffer.width);
	//viewport.height                       = -static_cast<float>(extend_of_gbuffer.height);        //viewport flipping
	//viewport.minDepth                     = 0.0f;
	//viewport.maxDepth                     = 1.0f;

	//g_buffer_dim.x��Ӧ |width| ,g_buffer_dim.y��Ӧ |height|,
	//�ӿڱ任��ʽ��ο�vulkan spec��
    const float ox = 0.f + g_buffer_dim.x / 2.0f;
    const float oy = g_buffer_dim.y + (-g_buffer_dim.y) / 2.0f;
    const float oz = 0.0f;

    const float px = g_buffer_dim.x;
    const float py = -g_buffer_dim.y;
    const float pz = 1.f;

	//������� 800x600 ��framebuffer��ʵ���ϵ���Ļ����ȡֵ��Χ��Ҳ����SV_POSITION��ֵ��Ϊ[0.5, 799.5]x[0.5, 599.5]
	//xf �� yf ��Ӧ��ȾGbufferʱ����Ļ�ռ����꣺
	//�����ǰ�x y��������ˣ�current PixelShader frag_coord -> current PixelShader frame buffer coord -> gubffer rendering frame buffer coord�任
    //��������ComputeNDCCoordFromFramebufferZcoord����Ĳ���zf����gubffer rendering frame buffer zf coord��ʵҲ��Ҫ�任������ͨ����Ⱦgbufferʱ����Ⱦscreen quadʱʹ�õ�����viewport��z�任��һ����
    const float xf = floor(  (float2(frag_coord).x + 0.5f) / current_RT_dim.x * g_buffer_dim.x  )  +  0.5f;
    const float yf = floor(  (float2(frag_coord).y + 0.5f) / current_RT_dim.y * g_buffer_dim.y  )  +  0.5f;

    const float x_ndc = (xf - ox) / (px / 2.f);
    const float y_ndc = (yf - oy) / (py / 2.f);
    const float z_ndc = (zf - oz) / pz;

    return float3(x_ndc, y_ndc, z_ndc);

    //const float z_view = ubo.projection[2][3] / -(ubo.projection[2][2] + z_ndc);

    //const float x_ndc = (xf - ox) / (px / 2.f);
    //float x_view = (x_ndc * (-z_view)) / ubo.projection[0][0];
    //const float y_ndc = (yf - oy) / (py / 2.f);
    //float y_view = (y_ndc * (-z_view)) / ubo.projection[1][1];

    //const float2 NDC_xy = float2(x_ndc, y_ndc);
    //const float4 pos_view = float4(x_view, y_view, z_view, 1.f);

    //�ؽ���ɣ�
    //const float3 pos_world = mul(ubo.view_inverse, pos_view).xyz;
}


float3 ComputeViewCoordFromNDCCoord(const float3 NDC_coord)
{

    const float z_view = ubo.projection[2][3] / -(ubo.projection[2][2] + NDC_coord.z);
    float x_view = (NDC_coord.x * (-z_view)) / ubo.projection[0][0];
    float y_view = (NDC_coord.y * (-z_view)) / ubo.projection[1][1];


    return float3(x_view, y_view, z_view);


}

float3 ComputeViewCoordFromViewZAndXyOfNDC(const float z_view, const float2 NDC_coord)
{
    float x_view = (NDC_coord.x * (-z_view)) / ubo.projection[0][0];
    float y_view = (NDC_coord.y * (-z_view)) / ubo.projection[1][1];
    return float3(x_view, y_view, z_view);
}






SurfaceData GetOneSampleSurfaceDataFromGBuffer(uint2 frag_coord, uint sampleIndex)
{
    // ��GBuffer��ȡ����
    // ����Ķ�ȡ���赱ǰPS��ȾĿ���ά�ȣ�������Gbuffer��һ���ģ�
    // TODO:�����һ������ô������ʱ�����
    SurfaceData result;
    result.normal = GNormal.Load(frag_coord.xy, sampleIndex).xyz;
    result.albedo = GAlbedo.Load(frag_coord.xy, sampleIndex).xyzw;
    result.pos = GPosition.Load(frag_coord.xy, sampleIndex).xyz;

    const float2 posZGrad = GposZGrad.Load(frag_coord.xy, sampleIndex).xy; //��frame space��X��Y����������view����ϵ�ڵ�zֵ���ݶȣ��仯ֵ������ͨ���������ڲ�õ��ġ�

    float2 g_buffer_dim;
    uint dummy;
    GPosition.GetDimensions(g_buffer_dim.x, g_buffer_dim.y, dummy);
	//��Ϊ���������composition pass�����ǵ�ǰ��PS��ʹ�õ�framebuffer��ά��ͨ�� ubo������������ֱ�Ӽ��� gbufferά�� �� ��ǰcomposition passʹ�õ�frambuffer buffer��ά��һ��
    const float2 current_render_target_dim = g_buffer_dim;
	//GDepth��������ȾGbuffer��geometry pass��ʱʹ�õ�depth attachment����������Load zֵʱʹ�õ������ǵ�ǰpixel shader��frame buffer frag coord��������˵Ӧ�����Ȱѵ�ǰpixel shader��frame buffer frag coord
    //ת��Ϊ��Ӧ��ȾGbufferʱ��Ӧ��frag coord��Ȼ����LOAD���������Ǽ����� gbufferά�� �� ��ǰcomposition passʹ�õ�frambuffer buffer��ά��һ��������ת�������ʡȥ�ˡ�
    const float zf = GDepth.Load(frag_coord.xy, sampleIndex).x;

    const float3 NDC_coord = ComputeNDCCoordFromFramebufferZcoord(current_render_target_dim, g_buffer_dim, frag_coord, zf);
    float3 view_coord = ComputeViewCoordFromNDCCoord(NDC_coord);
    float3 world_coord = mul(ubo.view_inverse, float4(view_coord, 1.0)).xyz;

    const float2 NDC_space_offset = float2(2.0f, -2.0f) / g_buffer_dim;
    const float2 NDCdx = NDC_coord.xy + float2(NDC_space_offset.x, 0.0f);
    const float2 NDCdy = NDC_coord.xy + float2(0.0f, NDC_space_offset.y);
    const float dx_z_view = view_coord.z + posZGrad[0];
    const float dy_z_view = view_coord.z + posZGrad[1];

    const float3 dx_view_coord = ComputeViewCoordFromViewZAndXyOfNDC(dx_z_view, NDCdx);
    const float3 dy_view_coord = ComputeViewCoordFromViewZAndXyOfNDC(dy_z_view, NDCdy);


	//�ؽ���ɣ�
    //const float3 pos_world = mul(ubo.view_inverse, pos_view).xyz;

	//dx_pos_world��dy_pos_world��ʾ ��gbuffer�ϵ�һ�������ϵ��������������� ��壨�����ڲ壩���������صõ�����������
    const float3 dx_pos_world = mul(ubo.view_inverse, float4(dx_view_coord, 1.0)).xyz;
    const float3 dy_pos_world = mul(ubo.view_inverse, float4(dy_view_coord, 1.0)).xyz;

    result.pos_DX = dx_pos_world - result.pos; //pos_DX���ֵ��ʾ��ֵ
    result.pos_DY = dy_pos_world - result.pos; //pos_DX���ֵ��ʾ��ֵ
    
    return result;
}



void GetAllSamplesSurfaceDataFromGBuffer(uint2 frame_buffer_coord,
                                         out SurfaceData samples_per_pixel[MSAA_SAMPLE_COUNT])
{
    // Ϊÿһ������������ȡ����
    for (uint i = 0; i < MSAA_SAMPLE_COUNT; ++i)
    {
        samples_per_pixel[i] = GetOneSampleSurfaceDataFromGBuffer(frame_buffer_coord, i);
    }



}



bool IsAComplexFragment(SurfaceData surface[MSAA_SAMPLE_COUNT])
{
    bool per_sample = false;

    const float maxZDelta = abs(surface[0].pos_DX.z) + abs(surface[0].pos_DY.z);
    const float minNormalDot = 0.99f; // �����Լ8�ȵķ��߽ǶȲ���

    for (uint i = 1; i < MSAA_SAMPLE_COUNT; ++i)
    {
        // ʹ�������ε�λ��ƫ�ƣ�������еĲ�����ȴ��ڲ���ϴ����������п������ڱ߽�
        per_sample = per_sample ||
            abs(surface[i].pos.z - surface[0].pos.z) > maxZDelta;

        // �����߽ǶȲ���ϴ����п������Բ�ͬ��������/����
        per_sample = per_sample ||
            dot(surface[i].normal, surface[0].normal) < minNormalDot;
    }

    return per_sample;
}





void main(float4 frame_buffer_coord : SV_Position) //���shader��render target��multisampled�ģ�
{

    SurfaceData samples_per_pixel[MSAA_SAMPLE_COUNT];
	// �������sv position��(0.5,0.5)����uint2ת������(0,0)���պñ�ʾ���ǵ�һ�е�һ�е����أ�Ҳ����frag coordinate
	// ��GBuffer��ȡ����
    // �����ȡ��ʱ����赱ǰPS��ȾĿ���ά�ȣ�������Gbuffer��һ���ģ�
    GetAllSamplesSurfaceDataFromGBuffer(uint2(frame_buffer_coord.xy), samples_per_pixel);

    //�ж��Ƿ�Ϊ���ӵ�����
    const bool complex_frag = IsAComplexFragment(samples_per_pixel);

	// ������ǲ���Ҫ�������ɫ������������ƬԪ(Ҳ���ǲ���ģ��ֵ����д��)
    if (!complex_frag)
    {
        discard;
    }


}











