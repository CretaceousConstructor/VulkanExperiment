#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable
//#extension GL_KHR_vulkan_glsl : enable


layout (location = 0) in vec3 inPos;

layout (binding = 0) uniform UBO 
{
	mat4 depthMVP;
} ubo;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

 
void main()
{
	gl_Position =  ubo.depthMVP * vec4(inPos, 1.0);
}