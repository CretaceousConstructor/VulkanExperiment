#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

layout (location = 0) out vec4 outFragColor;




void main() 
{

	//use const color for light indicator
	outFragColor = vec4(1.f,1.f,1.f,1.f);
}



