

struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 pos_l : TEXCUBECOORD;
};


[[vk::combinedImageSampler]][[vk::binding(1,0)]]
TextureCube textureCubeMap;
[[vk::combinedImageSampler]][[vk::binding(1,0)]]
SamplerState samplerCubeMap;


float4 main(VSOutput pIn) : SV_Target
{

    float3 color = textureCubeMap.Sample(samplerCubeMap, pIn.pos_l).xyz;
    return float4(color, 1.0);

}