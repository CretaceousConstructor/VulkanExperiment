

struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 pos_world : TEXCUBECOORD;
};

struct PushConsts
{
    [[vk::offset(0)]]  float4x4 model;
    [[vk::offset(64)]]	float4x4 pv;
    float roughness;
    uint numSamples;
};


[[vk::combinedImageSampler]][[vk::binding(0,0)]]
TextureCube environmentMap;
[[vk::combinedImageSampler]][[vk::binding(0,0)]]
SamplerState samplerEnvironmentMap;



[[vk::push_constant]] PushConsts push_consts;


#define PI 3.14159265358979323846
// Based omn http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
float random(float2 co)
{
    const float a = 12.9898;
    const float b = 78.233;
    const float c = 43758.5453;
    const float dt = dot(co.xy, float2(a, b));
    const float sn = fmod(dt, 3.14);
    return frac(sin(sn) * c);
}

float2 hammersley2d(uint i, uint N)
{
	// Radical inverse based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float rdi = float(bits) * 2.3283064365386963e-10;
    return float2(float(i) / float(N), rdi);
}

// Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
float3 importanceSample_GGX(float2 Xi, float roughness, float3 normal)
{
	// Maps a 2D point to a hemisphere with spread based on roughness
    const float alpha = roughness * roughness;
    const float phi = 2.0 * PI * Xi.x + random(normal.xz) * 0.1;
    const float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha * alpha - 1.0) * Xi.y));
    const float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    const float3 H = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	// Tangent space
    const float3 up = abs(normal.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 tangentX = normalize(cross(up, normal));
    const float3 tangentY = normalize(cross(normal, tangentX));

	// Convert to world Space
    return normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
}

// Normal Distribution function
float D_GGX(float dotNH, float roughness)
{
    const float alpha = roughness * roughness;
    const float alpha2 = alpha * alpha;
    const float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2) / (PI * denom * denom);
}

float3 prefilterEnvMap(float3 R, float roughness)
{
    const float3 N = R;
    const float3 V = R;

    float3 color = float3(0.0, 0.0, 0.0);
    float totalWeight = 0.0;
    int2 envMapDims;
    environmentMap.GetDimensions(envMapDims.x, envMapDims.y);
    const float envMapDim = float(envMapDims.x);


    for (uint i = 0u; i < push_consts.numSamples; i++)
    {
        const float2 Xi = hammersley2d(i, push_consts.numSamples);
        const float3 H = importanceSample_GGX(Xi, roughness, N);
        //const float3 L = 2.0 * dot(V, H) * H - V;
        const float3 L = reflect(-V, H);
        const float dotNL = clamp(dot(N, L), 0.0, 1.0);
        if (dotNL > 0.0)
        {
			// Filtering based on https://placeholderart.wordpress.com/2015/07/28/implementation-notes-runtime-environment-map-filtering-for-image-based-lighting/
            const float dotNH = clamp(dot(N, H), 0.0, 1.0);
            const float dotVH = clamp(dot(V, H), 0.0, 1.0);

			// Probability Distribution Function
            const float pdf = D_GGX(dotNH, roughness) * dotNH / (4.0 * dotVH) + 0.0001;
			// Solid angle of current sample
            const float omegaS = 1.0 / (float(push_consts.numSamples) * pdf);
			// Solid angle of 1 pixel across all cube faces
            const float omegaP = 4.0 * PI / (6.0 * envMapDim * envMapDim);
			// Biased (+1.0) mip level for better result
            const float mipLevel = roughness == 0.0 ? 0.0 : max(0.5 * log2(omegaS / omegaP) + 1.0, 0.0f);

            color += environmentMap.SampleLevel(samplerEnvironmentMap, L, mipLevel).rgb * dotNL;
            totalWeight += dotNL;

        }
    }
    return (color / totalWeight);
}


float4 main(VSOutput pIn) : SV_TARGET
{
    const float3 N = normalize(pIn.pos_world);
    return float4(prefilterEnvMap(N, push_consts.roughness), 1.0);

}
