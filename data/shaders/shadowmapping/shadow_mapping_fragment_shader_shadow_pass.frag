#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable


layout(location = 0) out vec4 color;

void main() 
{	
	color = vec4(1.0, 0.0, 0.0, 1.0);//pixel shader并不会被运行
}