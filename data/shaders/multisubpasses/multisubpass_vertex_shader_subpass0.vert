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
	//gltf中提供的法向量已经是model space中的向量了，而不是local model space中的向量，我们只需要对这些法向量做从model坐标系到world space的转换矩阵（此处暂时默认为单位矩阵）
	//实际上这里的 primitive.model 仅仅是吧子节点中所有的三角形放到model坐标系中，就是从local model space到model space。默认从model坐标系到world space的转换矩阵为单位矩阵,所以这里是primitive.model矩阵
	gl_Position = ubo_scene.proj * ubo_scene.view * primitive.model * vec4(inPos.xyz, 1.0);
	//由于这里从model坐标系到world space的转换矩阵为单位矩阵，所以不需要对inNormal 做转置逆变换
	
	outNormal =  (inverse(transpose(primitive.model)) * vec4( inNormal,1.0)).xyz;


	outColor = inColor;  //baseColorFactor value  
	outUV = inUV;

	vec4 world_pos = primitive.model * vec4(inPos, 1.0);
	outWoldPos = world_pos.xyz;



}

 