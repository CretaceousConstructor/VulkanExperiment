


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
    [[vk::location(0)]] float3 pos_l : TEXCUBECOORD;
};


struct PushConsts
{
    float4x4 model;
};


struct UBOScene
{
    float4x4 projection;
    float4x4 view;

    float3 light_pos;
    float3 light_color;
    float3 cam_pos;
    float exposure;
};



//[[vk::binding(X[, Y])]] can be attached to global variables to specify the descriptor set as Y and binding number as X.
[[vk::binding(0, 0)]] ConstantBuffer<UBOScene> ubo;
[[vk::push_constant]] PushConsts push_consts;


VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4x4 viewCancelTranslation = ubo.view;
    viewCancelTranslation[0][3] = 0.0;
    viewCancelTranslation[1][3] = 0.0;
    viewCancelTranslation[2][3] = 0.0;
    viewCancelTranslation[3][3] = 1.0;


    const float4x4 MVP = mul(ubo.projection, mul(viewCancelTranslation, push_consts.model));
    float4 posH = mul(MVP, float4(input.pos, 1.0f));

    output.pos_h = posH.xyww; //保持在最远
    output.pos_l = mul(push_consts.model, float4(input.pos, 1.0f)).xyz;


	// Convert cubemap coordinates into Vulkan coordinate space
    output.pos_l.xy *= -1.0f;

    return output;

}



