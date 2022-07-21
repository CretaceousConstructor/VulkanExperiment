#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable


layout (location = 0) out vec4 face_colors[6];
//layout (location = 1) out vec4 face_color_1;
//layout (location = 2) out vec4 face_color_2;
//layout (location = 3) out vec4 face_color_3;
//layout (location = 4) out vec4 face_color_4;
//layout (location = 5) out vec4 face_color_5;

  
layout (location = 0) in vec3 localPos;//world position做了 透视矫正插值以后得到的值。

layout (set = 0, binding = 1) uniform sampler2D equirectangularMap;

layout(push_constant) uniform 
PushConsts
{
	mat4 model;
	int face_id;
} primitive;

  
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
	vec2 uv = SampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
	vec3 color = texture(equirectangularMap, uv).rgb;
	face_colors[primitive.face_id]   = vec4(color, 1.0);
}




