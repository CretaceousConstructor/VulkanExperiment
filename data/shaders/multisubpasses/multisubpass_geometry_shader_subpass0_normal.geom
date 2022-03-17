#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable



layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout (set = 0,binding = 1) uniform UBO 
{
	mat4 projection;
	mat4 view;
} ubo;

layout (location = 0) in vec3 inNormal[];
layout (location = 1) in mat4 inModel[];




layout (location = 0) out vec3 outColor;

void main(void)
{	
	float normalLength = 0.02;
	for(int i=0; i< gl_in.length(); i++)
	{
		vec3 pos = gl_in[i].gl_Position.xyz;
		vec3 normal = inNormal[i].xyz;



		gl_Position = ubo.projection * (ubo.view * inModel[0] * vec4(pos, 1.0));
		outColor = vec3(1.0, 0.0, 0.0);
		EmitVertex();

		gl_Position = ubo.projection * (ubo.view * inModel[0] * vec4(pos + normal * normalLength, 1.0));
		outColor = vec3(0.0, 0.0, 1.0);
		EmitVertex();

		EndPrimitive();
	}
}