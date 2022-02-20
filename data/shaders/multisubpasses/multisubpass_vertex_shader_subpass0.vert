#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable


////uniform buffer
layout(set = 0,binding = 0) uniform  ShaderDataUBO{

	mat4 view;
	mat4 proj;
	vec4 lightPos;
	vec3 lightColor;
	vec3 eyePos;
	float ambientStrength;
	float specularStrength;

} ubo_scene;
//
layout(set = 0,binding = 1) uniform ShaderDataUBOTest {
	
	mat4 view;

} ubo_test;


layout(push_constant) uniform PushConsts {
	mat4 model;
} primitive;


////Input
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inUV;

//Output
layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outWoldPos;

//
//
void main() 
{

	 //float myfloat = 3.1415f;
	 //debugPrintfEXT("My float is %f", myfloat);
	 ////
	//gltf���ṩ�ķ������Ѿ���model space�е������ˣ�������local model space�е�����������ֻ��Ҫ����Щ����������model����ϵ��world space��ת�����󣨴˴���ʱĬ��Ϊ��λ����
	//ʵ��������� primitive.model �����ǰ��ӽڵ������е������ηŵ�model����ϵ�У����Ǵ�local model space��model space��Ĭ�ϴ�model����ϵ��world space��ת������Ϊ��λ����,����������primitive.model����
	gl_Position = ubo_scene.proj * ubo_scene.view * primitive.model * vec4(inPos.xyz, 1.0);
	//���������model����ϵ��world space��ת������Ϊ��λ�������Բ���Ҫ��inNormal ��ת����任
	
	outNormal =  (inverse(transpose(primitive.model)) * vec4( inNormal,1.0)).xyz;


	outColor = inColor;  //baseColorFactor value  
	outUV = inUV;

	vec4 world_pos = primitive.model * vec4(inPos, 1.0);
	outWoldPos = world_pos.xyz;



}

 