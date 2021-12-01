#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

//
////Input
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
//
////Output
//

layout(location = 0) out vec4 RoutColor;


////uniform 
layout(set = 0,binding = 2) uniform sampler2D texSampler;


void main() {
//
////    debugPrintfEXT("x");
////    debugPrintfEXT("%f", 1.0);
        vec4 outColor;
        outColor = texture(texSampler, fragTexCoord);
//
        RoutColor.r = outColor.r;
        RoutColor.g = outColor.r;
        RoutColor.b = outColor.r;

//
//
//  
}