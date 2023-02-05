struct VSInput
{
    [[vk::location(0)]] float4 tangent : TANGENT0;
    [[vk::location(1)]] float3 pos : POSITION0;
    [[vk::location(2)]] float3 normal : NORMAL0;
    [[vk::location(3)]] float3 color_factor : COLOR0;
    [[vk::location(4)]] float2 uv : TEXCOORD0;
};

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
    const float4x4 MVP = mul(ubo.projection, mul(ubo.view, push_consts.model));
    const float4 posH = mul(MVP, float4(input.pos, 1.0f));

    output.pos_h = posH;
    output.world_pos = mul(push_consts.model, float4(input.pos, 1.0f)).xyz;

	//normal    向量需要用model矩阵的 转置逆 来转换（但如果只是用了uniform scaling，则不需要转置逆）
    output.normal = mul(push_consts.model, float4(input.normal, 1.0)).xyz;
	//tangent   向量只需要用model矩阵转换?
    output.tangent = mul(push_consts.model, float4(input.tangent.xyz, 1.0)).xyz;
 
//    output.color_factor = input.color;

	//pos代表worldspace中顶点的坐标
    //float4 pos = mul(push_consts.model, float4(input.pos, 1.0));
	//outLightVec和outViewVec都会进行interpolation

    //output.view_vec = ubo.cam_pos.xyz - pos.xyz;
    //output.light_vec = ubo.light_pos.xyz - pos.xyz;

    output.uv = input.uv;
	//tangent向量的最后一个分量代表手性，这个值对一个三角形图元（primitive）来说只取 +1 或者 -1
    output.handness = input.tangent.w;



    return output;

}
