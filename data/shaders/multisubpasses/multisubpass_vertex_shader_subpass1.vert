#version 460
#extension GL_ARB_separate_shader_objects : enable

//#extension GL_KHR_vulkan_glsl : enable

#extension GL_EXT_debug_printf : enable

//
////#extension GL_KHR_vulkan_glsl : enable
//
out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{



	//vkCmdDraw(graphics_command_buffers[i], 3, 1, 0, 0);


	gl_Position = vec4(vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2) * 2.0f - 1.0f, 0.0f, 1.0f);





}     