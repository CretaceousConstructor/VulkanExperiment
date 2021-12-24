#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable
//#extension GL_KHR_vulkan_glsl : enable


layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	mat4 model;
	mat4 LightVP;
	vec3 lightPos;

} ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outWorldPos;
layout (location = 3) out vec3 worldposition;
layout (location = 4) out vec4 shadowCamPos;
layout (location = 5) out vec4 outHomo;




//layout (location = 2) out vec3 outViewVec;
//layout (location = 3) out vec3 outLightVec;
//layout (location = 4) out vec4 outShadowCoord;

out gl_PerVertex 
{
    vec4 gl_Position;   
};


void main() 
{
	outColor = inColor;
    outNormal = mat3(ubo.view) * mat3(ubo.model) * inNormal;
	outWorldPos = mat3(ubo.view) * mat3(ubo.model) * inPos;
	worldposition =  mat3(ubo.model) * inPos;
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPos, 1.0);

	
	vec4 shadow_world_coor  =  ubo.model * vec4(inPos, 1.0);
	vec4 shadow_HOMO_coor   =  ubo.LightVP * shadow_world_coor;

	outHomo = shadow_HOMO_coor;

	//PRE multiply W
	shadowCamPos = shadow_HOMO_coor * vec4(0.5f,-0.5f,1.f,1.f) + vec4(0.5f,0.5f,0.f,0.f) * shadow_HOMO_coor.w;
	



}

