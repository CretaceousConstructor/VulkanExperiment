#define LIGHTCOUNT 6
#define ambient 0.01

struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 world_pos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float3 tangent : TANGENT0;
    [[vk::location(3)]] float2 uv : TEXCOORD0;
    [[vk::location(4)]] float handness : HANDNESS;
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
    
    Light lights[6];
    float3 cam_pos;
};


//[[vk::binding(X[, Y])]] can be attached to global variables to specify the descriptor set as Y and binding number as X.

//set 0,binding 0
[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;

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



float4 main(VSOutput pIn) : SV_TARGET
{



    float3 fragcolor = float3(0.f, 0.f, 0.f);

	float4 albedo = albedoMap.Sample(samplerAlbedoMap, pIn.uv);

    float3 normal = calculateNormal(pIn.normal, pIn.tangent, pIn.uv, pIn.handness);
	// Ambient part
    fragcolor = albedo.rgb * ambient;

    for (int i = 0; i < LIGHTCOUNT; ++i)
    {
		// Vector to light
        float3 L = ubo.lights[i].position.xyz - pIn.world_pos;
		// Distance from light to fragment position
        const float dist = length(L);

		// Viewe vector to fragment
        float3 V = ubo.cam_pos.xyz - pIn.world_pos;
        V = normalize(V);

		//if(dist < ubo.lights[i].radius)
		//{
			// Light vector to fragment
        L = normalize(L);

			// Attenuation
        const float atten = ubo.lights[i].radius / (pow(dist, 2.0) + 1.0);

			// Diffuse part
        const float3 N = normalize(normal);
        const float NdotL = max(0.0, dot(N, L));
        const float3 diffuse = ubo.lights[i].color * albedo.rgb * NdotL * atten;

			// Specular part
			// Specular map values are stored in alpha of albedo mrt
        const float3 R = reflect(-L, N);
        const float NdotR = max(0.0, dot(R, V));
        const float3 spec = ubo.lights[i].color * albedo.a * pow(NdotR, 16.0) * atten;

        fragcolor += (diffuse + spec);
		//}
	}

    return float4(fragcolor, 1.0);


}
