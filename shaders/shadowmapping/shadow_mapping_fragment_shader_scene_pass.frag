#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable


layout (binding = 1) uniform sampler2D shadowMap;



layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inWorldPos;
layout (location = 3) in vec3 testWorldPos;
layout (location = 4) in vec4 inshadowCamPos;
layout (location = 5) in vec4 inHomo;

layout (location = 0) out vec4 outFragColor;


void main() 
{	


	vec4 temp_inshadowCamPos = inshadowCamPos;
	temp_inshadowCamPos /= vec4(inshadowCamPos.w,inshadowCamPos.w,inshadowCamPos.w,inshadowCamPos.w);
	float shadow_bias = 0.0005;
	vec4 shadow = texture(shadowMap, temp_inshadowCamPos.xy);//采样出[0,1]的值 

	if(shadow.r > temp_inshadowCamPos.z - shadow_bias){

		outFragColor = vec4(1.f,1.f,1.f,1.f);

	}
	else{

		outFragColor = vec4(0.1f,0.1f,0.1f,1.f);


	}


}
