


struct VSOutput
{
    float4 pos : SV_POSITION;
    [[vk::location(0)]] float3 world_pos : WORLDPOS; //world position做了 透视矫正插值以后得到的值。
    uint layer_index : SV_RenderTargetArrayIndex;
};


[[vk::combinedImageSampler]][[vk::binding(1,0)]]
Texture2D equirectangularMap;
[[vk::combinedImageSampler]][[vk::binding(1,0)]]
SamplerState equirectangularMapSampler;





float2 SampleSphericalMap(float3 v)
{

    const float2 invAtan = float2(0.1591, 0.3183);
    float2 uv = float2(atan2(v.x, v.z), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}


float4 main(VSOutput input) : SV_TARGET
{
    const float2 uv = SampleSphericalMap(normalize(input.world_pos)); // make sure to normalize WorldPos
    float3 color = equirectangularMap.Sample(equirectangularMapSampler, uv).xyz;
    return float4(color, 1.0);
}


