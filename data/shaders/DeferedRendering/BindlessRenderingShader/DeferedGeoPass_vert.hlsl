struct VSInput
{
    [[vk::location(0)]] float4 tangent : TANGENTCOORD0;
    [[vk::location(1)]] float3 pos : POSITION0;
    [[vk::location(2)]] float3 normal : NORMAL0;
    [[vk::location(3)]] float3 color : COLOR0;
    [[vk::location(4)]] float2 uv : TEXCOORD0;
};


struct VSOutput
{
    float4 pos_h : SV_POSITION;
    [[vk::location(0)]] float3 world_pos : POSITION0;
    [[vk::location(1)]] float3 normal : NORMAL0;
    [[vk::location(2)]] float3 tangent : TEXCOORD1;
    [[vk::location(3)]] float3 view_pos : VIEWPOSITION0;
    [[vk::location(4)]] float2 uv : TEXCOORD0;
    [[vk::location(5)]] float handness : HANDNESS;
};


struct PushConsts
{
    float4x4 model;
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
[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;
[[vk::push_constant]] PushConsts push_consts;

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    output.pos_h = mul(ubo.projection, mul(ubo.view, mul(push_consts.model, float4(input.pos, 1.0))));
	//normal    ������Ҫ��model����� ת���� ��ת���������ֻ������uniform scaling������Ҫת���棩
    output.normal = input.normal;
	//vertex position in world space
    output.world_pos = mul(push_consts.model, float4(input.pos, 1.)).xyz;
	//tangent   ����ֻ��Ҫ��model����ת��?
    output.tangent = input.tangent.xyz;

    output.view_pos = mul(ubo.view, mul(push_consts.model, float4(input.pos, 1.))).xyz;

    output.uv = input.uv;

    output.handness = input.tangent.w;

    return output;
}



