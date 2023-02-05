struct VSInput
{
    [[vk::location(0)]] float4 tangent : TEXCOORD1;
    [[vk::location(1)]] float3 pos : POSITION0;
    [[vk::location(2)]] float3 normal : NORMAL0;
    [[vk::location(3)]] float3 color : COLOR0;
    [[vk::location(4)]] float2 uv : TEXCOORD0;

};

struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 world_pos : WORLDPOS;
};


struct PushConsts
{
    [[vk::offset(0)]]  float4x4 model;
    [[vk::offset(64)]]	float4x4 pv;
    float roughness;
    uint numSamples;
};

[[vk::push_constant]] PushConsts push_consts;


VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    //suspicious: model matrix is for LR system or RH system?
    output.world_pos = mul(push_consts.model, float4(input.pos.xyz, 1.0)).xyz;
    output.pos_h = mul(push_consts.pv, mul(push_consts.model, float4(input.pos.xyz, 1.0)));

    return output;
}


