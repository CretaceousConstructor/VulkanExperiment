

#define PI 3.14159265358979323846

[[vk::constant_id(0)]] const uint NUM_SAMPLES = 1024u;


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
    float3 H = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	// Tangent space
    const float3 up = abs(normal.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 tangentX = normalize(cross(up, normal));
    const float3 tangentY = normalize(cross(normal, tangentX));

	// Convert to world Space
    return normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
}

// Geometric Shadowing function
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    const float k = (roughness * roughness) / 2.0;
    const float GL = dotNL / (dotNL * (1.0 - k) + k);
    const float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

float2 BRDF(float NoV, float roughness)
{
	// Normal always points along z-axis for the 2D lookup
    const float3 N = float3(0.0, 0.0, 1.0);
    float3 V = float3(sqrt(1.0 - NoV * NoV), 0.0, NoV);

    float2 LUT = float2(0.0, 0.0);

    for (uint i = 0u; i < NUM_SAMPLES; i++)
    {
        const float2 Xi = hammersley2d(i, NUM_SAMPLES);
        const float3 H = importanceSample_GGX(Xi, roughness, N);
        const float3 L = 2.0 * dot(V, H) * H - V;

        const float dotNL = max(dot(N, L), 0.0);
        const float dotNV = max(dot(N, V), 0.0);
        const float dotVH = max(dot(V, H), 0.0);
        const float dotNH = max(dot(H, N), 0.0);

        if (dotNL > 0.0)
        {
            const float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
            const float G_Vis = (G * dotVH) / (dotNH * dotNV);
            const float Fc = pow(1.0 - dotVH, 5.0);
            LUT += float2((1.0 - Fc) * G_Vis, Fc * G_Vis);
        }
    }
    return LUT / float(NUM_SAMPLES);
}




float4 main([[vk::location(0)]] float2 inUV : TEXCOORD0) : SV_TARGET
{
    return float4(BRDF(inUV.x, 1.0 - inUV.y), 0.0, 1.0);
}
