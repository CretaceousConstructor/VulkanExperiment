#version 460
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_debug_printf : enable
//Input
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

//Output
layout(location = 0) out vec4 outColor;


//uniform
layout(binding = 2) uniform sampler2D texSampler;

void main() {

//    debugPrintfEXT("x");
//    debugPrintfEXT("%f", 1.0);
  
    outColor = texture(texSampler, fragTexCoord);



}