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
	//tangent����ֻ��Ҫ��model����ת��	
	outTangent = mat3(primitive.model) * vec3(inTangent);
	//normal������Ҫ��model����� ת���� ��ת��
	outNormal = transpose(inverse(mat3(primitive.model))) * inNormal;
	//tangent���������һ�������������ԣ����ֵ��һ�������Σ�primitive����˵ֻȡ +1 ���� -1	
	outHandedness = inTangent.w;






	gl_Position = uboScene.projection * uboScene.view * primitive.model * vec4(inPos.xyz, 1.0);
	//pos����worldspace�ж��������
	vec4 pos = primitive.model * vec4(inPos, 1.0);
	//outLightVec��outViewVec�������interpolation
	outLightVec = uboScene.lightPos.xyz - pos.xyz;
	outViewVec = uboScene.viewPos.xyz - pos.xyz;


}
