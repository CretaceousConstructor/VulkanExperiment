#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable


layout (set = 1, binding = 0) uniform sampler2D samplerColorMap;
layout (set = 1, binding = 1) uniform sampler2D samplerNormalMap;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inTangent;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inViewVec;
layout (location = 4) in vec3 inLightVec;
layout (location = 5) in vec2 inUV;
layout (location = 6) in float inHandedness;

layout (location = 0) out vec4 outFragColor;

layout (constant_id = 0) const bool ALPHA_MASK = false;
layout (constant_id = 1) const float ALPHA_MASK_CUTOFF = 0.0f;

void main() 
{



	vec4 color = texture(samplerColorMap, inUV) * vec4(inColor, 1.0);
	
	if (ALPHA_MASK) {
		if (color.a < ALPHA_MASK_CUTOFF) {
			discard;
		}
	}

	vec3 N = normalize(inNormal);
	//施密特正交化：T向量在经过插值以后，可能不再和N向量相互垂直
	vec3 T = normalize(inTangent.xyz - dot(inTangent.xyz, N) * N);
	vec3 B = cross(N,T) * inHandedness;
	mat3 TBN = mat3(T, B, N);

	N = TBN * normalize(texture(samplerNormalMap, inUV).xyz * 2.0 - vec3(1.0));


	vec3 L = normalize(inLightVec);
	vec3 V = normalize(inViewVec);
	vec3 R = reflect(-L, N);

	
	
	
	const float ambient = 0.01;
	vec3 diffuse = max(dot(N, L),0).rrr;
	float specular = 0.0;
	if( dot(N, L) > 0.){
		specular = pow(max(dot(R, V), 0.0), 32.0);
	}


	outFragColor = vec4((diffuse+ specular + ambient) * color.rgb , color.a);




}
