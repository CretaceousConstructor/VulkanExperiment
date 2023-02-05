

struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 pos_l : TEXCUBECOORD;
};


[[vk::combinedImageSampler]][[vk::binding(1,0)]]
TextureCube textureCubeMap;
[[vk::combinedImageSampler]][[vk::binding(1,0)]]
SamplerState samplerCubeMap;


struct UBOScene
{
    float4x4 projection;
    float4x4 view;

    float3 light_pos;
    float3 light_color;
    float3 cam_pos;
    float exposure;
};



float3 Uncharted2Tonemap(float3 color)
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    const float W = 11.2;
    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}


[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;

float4 main(VSOutput pIn) : SV_Target
{

    float3 color = textureCubeMap.Sample(samplerCubeMap, pIn.pos_l).xyz;

	// Tone mapping
    color = Uncharted2Tonemap(color * ubo.exposure);
    color = color * (1.0f / Uncharted2Tonemap((11.2f).xxx));

    return float4(color, 1.0);

}