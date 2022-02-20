#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable


layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out mat4 outModel;

layout(push_constant) uniform PushConsts {
	mat4 model;
} primitive;


void main(void)
{


	outNormal = inNormal;
	outModel = primitive.model;
	gl_Position = vec4(inPos.xyz, 1.0);
	//local model space



}