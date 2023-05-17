struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 world_pos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float3 tangent : TANGENT0;
    [[vk::location(3)]] float3 view_pos : VIEWPOSITION0;
    [[vk::location(4)]] float2 uv : TEXCOORD0;
    [[vk::location(5)]] float handness : HANDNESS;
};




struct FSOutput
{
    float4 Position : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 Albedo : SV_TARGET2;
    float2 posZGrad : SV_TARGET3;
};

	



//set 1,binding 0
[[vk::combinedImageSampler]][[vk::binding(0,1)]]
Texture2D albedoMap;
[[vk::combinedImageSampler]][[vk::binding(0,1)]]
SamplerState samplerAlbedoMap;

//set 1,binding 1
[[vk::combinedImageSampler]][[vk::binding(1,1)]]
Texture2D normalMap;
[[vk::combinedImageSampler]][[vk::binding(1,1)]]
SamplerState samplerNormalMap;



float3 calculateNormal(float3 pIn_normal, float3 pIn_tangent, float2 pIn_uv, float handness)
{
    float3 N = normalize(pIn_normal);
	//施密特正交化：T向量在经过插值以后，可能不再和N向量相互垂直
    float3 T = normalize(pIn_tangent - dot(pIn_tangent, N) * N);

    float3 B = normalize(cross(N, T) * handness);
    const float3x3 TBN = transpose(float3x3(T, B, N));
    N = mul(TBN, normalize(normalMap.Sample(samplerNormalMap, pIn_uv).xyz * 2.0 - float3(1.0, 1.0, 1.0)));

    return N;
}



FSOutput main(VSOutput pIn)
{

    FSOutput output = (FSOutput) 0;

    output.Position = float4(pIn.world_pos, 1.0);
    const float3 N = calculateNormal(pIn.normal, pIn.tangent, pIn.uv, pIn.handness);
    output.Normal = float4(N, 1.0);
    output.Albedo = albedoMap.Sample(samplerAlbedoMap, pIn.uv);

	//TODO:这里应该往里面写view空间的pIn.view_pos.z不然之后会很难处理！
    output.posZGrad =
						float2(ddx_coarse(pIn.view_pos.z),
							   ddy_coarse(pIn.view_pos.z));


    return output;
}