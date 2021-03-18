#version 460
#extension GL_ARB_separate_shader_objects : enable
//#extension GL_EXT_debug_printf : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;

void main() {

//    debugPrintfEXT("x");
//    debugPrintfEXT("%f", 1.0);
    outColor = vec4(fragColor, 1.0);
}