#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

layout (location = 0) in vec3 inPos;


layout (set = 0, binding = 0) uniform UBOScene 
{
	mat4 projection;
	mat4 view;
	vec4 lightPos;
	vec3 viewPos;
} ubo_scene;


layout(push_constant) uniform PushConsts {
	mat4 model;
} primitive;


void main() 
{


	gl_Position = ubo_scene.projection * ubo_scene.view *primitive.model * vec4(inPos.xyz, 1.0);

}
