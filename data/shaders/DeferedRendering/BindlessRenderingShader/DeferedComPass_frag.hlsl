#define LIGHTCOUNT 6
#define ambient 0.01



struct VSOutput
{
    float4 posViewport : SV_Position;
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
[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;

//set 0,binding 1
[[vk::combinedImageSampler]][[vk::binding(1,0)]]
Texture2D GPosition;
[[vk::combinedImageSampler]][[vk::binding(1,0)]]
SamplerState samplerGPosition;

//set 0,binding 2
[[vk::combinedImageSampler]][[vk::binding(2,0)]]
Texture2D GNormal;
[[vk::combinedImageSampler]][[vk::binding(2,0)]]
SamplerState samplerGNormal;


//set 0,binding 3
[[vk::combinedImageSampler]][[vk::binding(3,0)]]
Texture2D GAlbedo;
[[vk::combinedImageSampler]][[vk::binding(3,0)]]
SamplerState samplerGAlbedo;



//set 0,binding 4, high precision depth value 
[[vk::combinedImageSampler]][[vk::binding(4,0)]]
Texture2D GDepth;
[[vk::combinedImageSampler]][[vk::binding(4,0)]]
SamplerState samplerGDepth;



float4 main(VSOutput pIn) : SV_TARGET
{


	// Get G-Buffer values
    const float3 fragPos = GPosition.Sample(samplerGPosition, pIn.uv).rgb;
    const float3 normal = GNormal.Sample(samplerGNormal, pIn.uv).rgb;
    const float4 albedo = GAlbedo.Sample(samplerGAlbedo, pIn.uv);



    float3 fragcolor = float3(0.f, 0.f, 0.f);

	// Ambient part
    fragcolor = albedo.rgb * ambient;

    for (int i = 0; i < LIGHTCOUNT; ++i)
    {
		// Vector to light
        float3 L = ubo.lights[i].position.xyz - fragPos;
		// Distance from light to fragment position
        const float dist = length(L);

		// Viewe vector to fragment
        float3 V = ubo.cam_pos.xyz - fragPos;
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