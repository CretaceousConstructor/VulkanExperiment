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

    //这里的视口逆变换用的是gbuffer渲染时的视口。如果用current render target的视口实际上也可以，就不用做 当前的framebuffer坐标到渲染gbuffer时的framebuffer坐标的变换了
	//进行视口逆变换

	//gbuffer渲染时的默认视口：假设在渲染G-buffer时使用的view port和G-buffer维度一样的话，那么下面的extend_of_gbuffer和g_buffer_dim就一样
	//VkViewport       viewport{};
	//const VkExtent3D extend_of_gbuffer = swapchain_manager.GetSwapChainImageExtent();
	//viewport.x                            = 0.0f;
	//viewport.y                            = static_cast<float>(extend_of_gbuffer.height);
	//viewport.width                        = static_cast<float>(extend_of_gbuffer.width);
	//viewport.height                       = -static_cast<float>(extend_of_gbuffer.height);        //viewport flipping
	//viewport.minDepth                     = 0.0f;
	//viewport.maxDepth                     = 1.0f;

	//g_buffer_dim.x对应 |width| ,g_buffer_dim.y对应 |height|,
	//视口变换公式请参考vulkan spec：
    const float ox = 0.f + g_buffer_dim.x / 2.0f;
    const float oy = g_buffer_dim.y + (-g_buffer_dim.y) / 2.0f;
    const float oz = 0.0f;

    const float px = g_buffer_dim.x;
    const float py = -g_buffer_dim.y;
    const float pz = 1.f;

	//比如对于 800x600 的framebuffer，实际上的屏幕坐标取值范围（也就是SV_POSITION的值）为[0.5, 799.5]x[0.5, 599.5]
	//xf 和 yf 对应渲染Gbuffer时的屏幕空间坐标：
	//下面是把x y坐标进行了：current PixelShader frag_coord -> current PixelShader frame buffer coord -> gubffer rendering frame buffer coord变换
    //而本函数ComputeNDCCoordFromFramebufferZcoord传入的参数zf就是gubffer rendering frame buffer zf coord其实也需要变换，但是通常渲染gbuffer时和渲染screen quad时使用的两个viewport的z变换是一样的
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

    //重建完成：
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
    // 从GBuffer读取数据
    // 这里的读取假设当前PS渲染目标的维度（长宽）和Gbuffer是一样的，
    // TODO:如果不一样该怎么处理，暂时不想管
    SurfaceData result;
    result.normal = GNormal.Load(frag_coord.xy, sampleIndex).xyz;
    result.albedo = GAlbedo.Load(frag_coord.xy, sampleIndex).xyzw;
    result.pos = GPosition.Load(frag_coord.xy, sampleIndex).xyz;

    const float2 posZGrad = GposZGrad.Load(frag_coord.xy, sampleIndex).xy; //在frame space上X和Y两个方向上view坐标系内的z值的梯度（变化值），是通过外插或者内插得到的。

    float2 g_buffer_dim;
    uint dummy;
    GPosition.GetDimensions(g_buffer_dim.x, g_buffer_dim.y, dummy);
	//因为懒，不想把composition pass（就是当前的PS）使用的framebuffer的维度通过 ubo传进来，所以直接假设 gbuffer维度 和 当前composition pass使用的frambuffer buffer的维度一样
    const float2 current_render_target_dim = g_buffer_dim;
	//GDepth就是在渲染Gbuffer（geometry pass）时使用的depth attachment。但是这里Load z值时使用的坐标是当前pixel shader的frame buffer frag coord，按理来说应该是先把当前pixel shader的frame buffer frag coord
    //转换为对应渲染Gbuffer时对应的frag coord，然后再LOAD，但是我们假设了 gbuffer维度 和 当前composition pass使用的frambuffer buffer的维度一样，所以转换步骤就省去了。
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


	//重建完成：
    //const float3 pos_world = mul(ubo.view_inverse, pos_view).xyz;

	//dx_pos_world和dy_pos_world表示 在gbuffer上的一个像素上的样本的世界坐标 外插（或者内插）到相邻像素得到的世界坐标
    const float3 dx_pos_world = mul(ubo.view_inverse, float4(dx_view_coord, 1.0)).xyz;
    const float3 dy_pos_world = mul(ubo.view_inverse, float4(dy_view_coord, 1.0)).xyz;

    result.pos_DX = dx_pos_world - result.pos; //pos_DX这个值表示阈值
    result.pos_DY = dy_pos_world - result.pos; //pos_DX这个值表示阈值
    
    return result;
}



void GetAllSamplesSurfaceDataFromGBuffer(uint2 frame_buffer_coord,
                                         out SurfaceData samples_per_pixel[MSAA_SAMPLE_COUNT])
{
    // 为每一个采样样本读取数据
    for (uint i = 0; i < MSAA_SAMPLE_COUNT; ++i)
    {
        samples_per_pixel[i] = GetOneSampleSurfaceDataFromGBuffer(frame_buffer_coord, i);
    }



}



bool IsAComplexFragment(SurfaceData surface[MSAA_SAMPLE_COUNT])
{
    bool per_sample = false;

    const float maxZDelta = abs(surface[0].pos_DX.z) + abs(surface[0].pos_DY.z);
    const float minNormalDot = 0.99f; // 允许大约8度的法线角度差异

    for (uint i = 1; i < MSAA_SAMPLE_COUNT; ++i)
    {
        // 使用三角形的位置偏移，如果所有的采样深度存在差异较大的情况，则有可能属于边界
        per_sample = per_sample ||
            abs(surface[i].pos.z - surface[0].pos.z) > maxZDelta;

        // 若法线角度差异较大，则有可能来自不同的三角形/表面
        per_sample = per_sample ||
            dot(surface[i].normal, surface[0].normal) < minNormalDot;
    }

    return per_sample;
}





void main(float4 frame_buffer_coord : SV_Position) //这个shader的render target是multisampled的！
{

    SurfaceData samples_per_pixel[MSAA_SAMPLE_COUNT];
	// 这里比如sv position是(0.5,0.5)经过uint2转换后是(0,0)，刚好表示的是第一行第一列的像素，也就是frag coordinate
	// 从GBuffer读取数据
    // 这里读取的时候假设当前PS渲染目标的维度（长宽）和Gbuffer是一样的，
    GetAllSamplesSurfaceDataFromGBuffer(uint2(frame_buffer_coord.xy), samples_per_pixel);

    //判断是否为复杂的像素
    const bool complex_frag = IsAComplexFragment(samples_per_pixel);

	// 如果我们不需要逐采样着色，抛弃该像素片元(也就是不对模板值进行写入)
    if (!complex_frag)
    {
        discard;
    }


}











