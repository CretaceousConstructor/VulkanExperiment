


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
    uint layer_index : SV_RenderTargetArrayIndex;
};


struct PushConsts
{
    float4x4 model;
};


struct UBOScene
{
    float4x4 projection;
    float4x4 view[6];
};



////[[vk::binding(X[, Y])]] can be attached to global variables to specify the descriptor set as Y and binding number as X.
///
///
///
[[vk::binding(0,0)]] ConstantBuffer<UBOScene> ubo;
[[vk::push_constant]] PushConsts push_consts;
//
//
//
//
VSOutput main(VSInput input, uint vid : SV_VertexID)
{
    VSOutput output = (VSOutput) 0;
    const uint faceId = (vid / 3) % 6;
    uint layerIndex = 0;

    switch (faceId)
    {
        case 0:
            layerIndex = 3;
            break;
        case 1:
            layerIndex = 2;
            break;
        case 2:
            layerIndex = 0;
            break;
        case 3:
            layerIndex = 4;
            break;
        case 4:
            layerIndex = 1;
            break;
		case 5:
			layerIndex = 5;
			break;
    }

    output.layer_index = layerIndex;


    //suspicious: model matrix is for LR system or RH system?
    output.world_pos = mul(push_consts.model,float4(input.pos.xyz, 1.0)).xyz;

    const float4x4 viewModel= mul(ubo.view[layerIndex],push_consts.model);
    output.pos_h = mul(ubo.projection,mul(viewModel,float4(input.pos.xyz, 1.0)));


    return output;
}


