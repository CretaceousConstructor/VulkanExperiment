


struct UBOScene
{
    float4x4 projection;
    float4x4 view;

    float3 light_pos;
    float3 light_color;
    float3 cam_pos;
    float exposure;
};
//
//
//
//
struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 world_pos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float3 tangent : TANGENT0;
    //[[vk::location(3)]] float3 color_factor : COLOR0;
    [[vk::location(3)]] float2 uv : TEXCOORD0;
    [[vk::location(4)]] float handness : HANDNESS;
};





//set 0,binding 2
[[vk::combinedImageSampler]][[vk::binding(2,0)]]
TextureCube irradianceMap;
[[vk::combinedImageSampler]][[vk::binding(2,0)]]
SamplerState samplerIrradianceMap;



//set 0,binding 3
[[vk::combinedImageSampler]][[vk::binding(3,0)]]
TextureCube prefilteredMap;
[[vk::combinedImageSampler]][[vk::binding(3,0)]]
SamplerState samplerPrefilteredMap;



//set 0,binding 4
[[vk::combinedImageSampler]][[vk::binding(4,0)]]
Texture2D BRDFLUTMap;
[[vk::combinedImageSampler]][[vk::binding(4,0)]]
SamplerState samplerBRDFLUTMap;



//set 1,binding 0
[[vk::combinedImageSampler]][[vk::binding(0,1)]]
Texture2D albedo;
[[vk::combinedImageSampler]][[vk::binding(0,1)]]
SamplerState samplerAlbedo;


//set 1,binding 1
[[vk::combinedImageSampler]][[vk::binding(1,1)]]
Texture2D normalMap;
[[vk::combinedImageSampler]][[vk::binding(1,1)]]
SamplerState samplerNormalMap;


//set 1,binding 2
[[vk::combinedImageSampler]][[vk::binding(2,1)]]
Texture2D aoMap;
[[vk::combinedImageSampler]][[vk::binding(2,1)]]
SamplerState samplerAoMap;


//set 1,binding 3
[[vk::combinedImageSampler]][[vk::binding(3,1)]]
Texture2D metallicRoughness;
[[vk::combinedImageSampler]][[vk::binding(3,1)]]
SamplerState samplerMetallicRoughness;





[[vk::constant_id(0)]] bool ALPHA_MASK = false;
[[vk::constant_id(1)]] float ALPHA_MASK_CUTOFF = 0.f;



[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;



#define PI 3.14159265358979323846

float DistributionGGX(float3 N, float3 H, float roughness)
{

    const float a = roughness * roughness;
    const float a2 = a * a;
    const float NdotH = max(dot(N, H), 0.0);
    const float NdotH2 = NdotH * NdotH;
	
    const float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
    const float r = (roughness + 1.0);
    const float k = (r * r) / 8.0;

    const float num = NdotV;
    const float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    const float NdotV = max(dot(N, V), 0.0);
    const float NdotL = max(dot(N, L), 0.0);
    const float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    const float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}




float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max((1.0 - roughness).xxx, F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}




float3 calculateNormal(float3 pIn_normal, float3 pIn_tangent, float2 pIn_uv, float handness)
{
	
    float3 N = normalize(pIn_normal);
	//施密特正交化：T向量在经过插值以后，可能不再和N向量相互垂直
    float3 T = normalize(pIn_tangent - dot(pIn_tangent, N) * N);
    float3 B = cross(N, T) * handness;
    const float3x3 TBN = transpose(float3x3(T, B, N));
    N = mul(TBN, normalize(normalMap.Sample(samplerNormalMap, pIn_uv).xyz * 2.0 - float3(1.0, 1.0, 1.0)));
    return N;
}



float3 Uncharted2Tonemap(float3 x)
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}




float4 main(VSOutput pIn) : SV_Target
{


    const float4 albedo_color = albedo.Sample(samplerAlbedo, pIn.uv);
    if (ALPHA_MASK)
    {
        if (albedo_color.a < ALPHA_MASK_CUTOFF)
        {
            discard;
        }
    }

	//sampling
    float4 sampled_metallic_roughness = metallicRoughness.Sample(samplerMetallicRoughness, pIn.uv);
    const float roughness = sampled_metallic_roughness.g;
    const float metallic = sampled_metallic_roughness.b;
    const float4 ao = aoMap.Sample(samplerAoMap, pIn.uv);

	//calculate normal
    const float3 N = calculateNormal(pIn.normal, pIn.tangent, pIn.uv, pIn.handness);

	//fresnel term
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo_color.xyz, metallic);

	//radiance
    float3 Lo = float3(0.0, 0.0, 0.0);

//DIRECT LUMINATION:
	//loop direct lights here:
    const float3 Wi = normalize(ubo.light_pos - pIn.world_pos); //light L
    const float3 Wo = normalize(ubo.cam_pos - pIn.world_pos); //view V
    const float3 H = normalize(Wo + Wi);

	//因为天空盒在绘制的时候采用了x和y取相反数（不然天空盒是反的）
    const float3 sampled_Wo = float3(-Wo.x, -Wo.y, Wo.z);
    const float3 sampled_N = float3(-N.x, -N.y, N.z);
    const float3 sampled_R = normalize(reflect(-sampled_Wo, sampled_N));

    if (dot(N, Wi) > 0.0)
    {
        const float distance = length(ubo.light_pos - pIn.world_pos);
        const float attenuation = 1.0 / (distance * distance);
        const float3 radiance = ubo.light_color * attenuation;


		//cook-torrance brdf
        const float NDF = DistributionGGX(N, H, roughness);
        const float G = GeometrySmith(N, Wo, Wi, roughness);
        const float3 F = fresnelSchlick(max(dot(H, Wo), 0.0), F0);

        const float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;

		//金属性越强，diffuse light被吸收的越多
        kD *= (1.0 - metallic);

        const float3 numerator = NDF * G * F;
        const float denominator = 4.0 * max(dot(N, Wo), 0.0) * max(dot(N, Wi), 0.0) + 0.0001;
        const float3 specular = numerator / denominator;

        const float NdotL = max(dot(N, Wi), 0.0);
        Lo += ((kD * albedo_color.xyz / PI + specular) * radiance * NdotL);
    }


//INDIRECT LUMINATION:
    const float3 F = fresnelSchlickRoughness(max(dot(N, Wo), 0.0), F0, roughness);
    const float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= (1.0 - metallic);
	//diffuse part
    const float3 irradiance = irradianceMap.Sample(samplerIrradianceMap, sampled_N).xyz;
    const float3 diffuse = irradiance * albedo_color.xyz;
	//specular part
    float2 brdf = BRDFLUTMap.Sample(samplerBRDFLUTMap, float2(max(dot(N, Wo), 0.0), roughness)).rg;
    const float3 left_part = prefilteredMap.SampleLevel(samplerPrefilteredMap, sampled_R, roughness).rgb;
    const float3 specular = left_part * (F * brdf.x + brdf.y);
	//ambient part
    const float3 LoIndirect = (kD * diffuse + specular) * aoMap.Sample(samplerAoMap, pIn.uv).rrr;
//FIANL COLOR
    float3 color = LoIndirect + Lo;

    //TONE MAPPING
    color = Uncharted2Tonemap(color * ubo.exposure);
    color = color * (1.0f / Uncharted2Tonemap((11.2f).xxx));

    //needless to do gamma correction because framebuffer is in srgb space, hardware will take care of it.
    return float4(color, albedo_color.w);
}






