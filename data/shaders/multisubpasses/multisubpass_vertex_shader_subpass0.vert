#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

//#extension GL_KHR_vulkan_glsl : enable


layout(set = 0,binding = 0) uniform  UniformBufferObject{

    mat4 model;
    mat4 view;
    mat4 proj;
} ubo1;
//
layout(set = 0,binding = 1) uniform UniformBufferObjectTest {
    mat4 model;
    mat4 view;

} uboShit;
//
//
//
//
//
////Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
//layout(location = 3) in mat4 instanceMatrix;
////Output
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
//
//
void main() 
{

     //float myfloat = 3.1415f;
     //debugPrintfEXT("My float is %f", myfloat);
////
    //以下三个值均进行了interpolation
//     gl_Position = ubo1.proj * uboShit.view  * uboShit.model* instanceMatrix * vec4(inPosition, 1.0);
     gl_Position = ubo1.proj * uboShit.view  * uboShit.model*  vec4(inPosition, 1.0);
     fragColor = inColor;
     fragTexCoord = inTexCoord;
//
}
//
//
 