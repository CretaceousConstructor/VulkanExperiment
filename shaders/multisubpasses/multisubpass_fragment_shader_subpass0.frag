#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

//
////Input
layout(location = 0) in vec3 fragColor;   //interpolation之后得到的值
layout(location = 1) in vec2 fragTexCoord;//interpolation之后得到的值
//
////Output
//

layout(location = 0) out vec4 RoutColor;


////uniform 
layout(set = 0,binding = 2) uniform sampler2D CombinedTexSampler_human_face;
layout(set = 0,binding = 3) uniform sampler2D CombinedTexSampler_viking_room;


void main() {





        vec4 outColor;
        outColor = texture(CombinedTexSampler_viking_room, fragTexCoord);
        
//      
//        
//        debugPrintfEXT("interpolated color = %v2f and corresponding color = = %v4f",fragTexCoord,outColor);
//     
//   

        RoutColor = outColor;
    

//
//
//  
}