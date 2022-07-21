
#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable
#extension GL_ARB_gpu_shader_int64 : enable


layout (location = 0) in vec4 inTangent;
layout (location = 1) in vec3 inPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec2 inUV;


layout (location = 0) out vec3 localPos;

layout (set = 0, binding = 0) uniform  
UBOScene 
{
	mat4 projection;
	mat4 view[6];
}ubo_scene;


layout(push_constant) uniform 
PushConsts
{
	mat4 model;
	int face_id;
} primitive;



void main()
{

	localPos = vec3(primitive.model * vec4(inPos,1.0));  
    gl_Position =  ubo_scene.projection * ubo_scene.view[primitive.face_id] * primitive.model * vec4(localPos, 1.0);


}
