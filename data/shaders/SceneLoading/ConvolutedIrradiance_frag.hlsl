

struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 pos_world : TEXCUBECOORD;
};


[[vk::combinedImageSampler]][[vk::binding(1,0)]]
TextureCube environmentMap;
[[vk::combinedImageSampler]][[vk::binding(1,0)]]
SamplerState samplerEnvironmentMap;




float4 main(VSOutput pIn) : SV_Target
{
    // the sample direction equals the hemisphere's orientation

	const float PI = 3.14159265358979323846;
    const float3 normal = normalize(pIn.pos_world);
  
    float3 irradiance = float3(0., 0., 0.);
  
    // convolution code
    float3 up = float3(0.0, 1.0, 0.0);
    const float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    const float deltaPhi = (2.0f * PI) / 180.0f;
    const float deltaTheta = (0.5f * PI) / 64.0f;

    //const float deltaPhi = 0.025;
    uint nrSamples = 0u;
    for (float phi = 0.0; phi < 2.0 * PI; phi += deltaPhi)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += deltaTheta)
        {
			// spherical to cartesian (in tangent space)
            const float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world space
            const float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            irradiance += environmentMap.Sample(samplerEnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }


    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    return float4(irradiance, 1.0);

}