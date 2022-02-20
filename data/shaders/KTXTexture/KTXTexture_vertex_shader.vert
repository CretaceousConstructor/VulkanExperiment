
#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (set = 0 ,binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	vec4 eyepos;
	float lodBias;
} ubo;

layout (location = 0) out vec2 outUV;
layout (location = 1) out float outLodBias;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main() 
{
	outUV = inUV;
	outLodBias = ubo.lodBias;
	//简单模型，没有传入model matrix
	mat4 model =  mat4(1.0);
	gl_Position = ubo.projection * ubo.view * model * vec4(inPos.xyz, 1.0);
	//所有运算在worldspace进行
	//pos in world space
    vec4 pos = model * vec4(inPos, 1.0);
	//normal in world space
	outNormal = mat3(inverse(transpose(model))) * inNormal;
	//assume light at (0,0,0)
	vec3 lightPos = vec3(0.0);
	//light positon in world space
	vec3 lPos =  lightPos.xyz;
    outLightVec = lPos - pos.xyz;
    outViewVec = ubo.eyepos.xyz - pos.xyz;		
}
