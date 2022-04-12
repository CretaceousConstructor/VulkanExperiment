#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable



layout (location = 0) in vec4 inTangent;
layout (location = 1) in vec3 inPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inColor;
layout (location = 4) in vec2 inUV;



layout (set = 0, binding = 0) uniform UBOScene 
{
	mat4 projection;
	mat4 view;
	vec4 lightPos;
	vec4 viewPos;
} uboScene;

layout(push_constant) uniform PushConsts {
	mat4 model;
} primitive;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outTangent;
layout (location = 2) out vec3 outColor;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;
layout (location = 5) out vec2 outUV;
layout (location = 6) out float outHandedness;

void main() 
{
	outColor = inColor;
	outUV = inUV;
	//tangent向量只需要用model矩阵转换	
	outTangent = mat3(primitive.model) * vec3(inTangent);
	//normal向量需要用model矩阵的 转置逆 来转换
	outNormal = transpose(inverse(mat3(primitive.model))) * inNormal;
	//tangent向量的最后一个分量代表手性，这个值对一个三角形（primitive）来说只取 +1 或者 -1	
	outHandedness = inTangent.w;






	gl_Position = uboScene.projection * uboScene.view * primitive.model * vec4(inPos.xyz, 1.0);
	//pos代表worldspace中顶点的坐标
	vec4 pos = primitive.model * vec4(inPos, 1.0);
	//outLightVec和outViewVec都会进行interpolation
	outLightVec = uboScene.lightPos.xyz - pos.xyz;
	outViewVec = uboScene.viewPos.xyz - pos.xyz;


}
