#version 460
#extension GL_ARB_separate_shader_objects : enable

//#extension GL_KHR_vulkan_glsl : enable

#extension GL_EXT_debug_printf : enable


layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(set = 0,binding = 1) uniform UniformBufferObjectTest {
    mat4 model;
    mat4 view;

} uboShit;






layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in mat4 instanceMatrix;

layout(location = 0) out vec3 fragColor;

void main() 
{

//        debugPrintfEXT("Here are two float values \n");
//
//       debugPrintfEXT("Here's a smaller float value \n");
//
//      debugPrintfEXT("Here's an float %f with text before and after it\n",  uboShit.model[0][0]);
//
  
    gl_Position = ubo.proj * uboShit.view  * uboShit.model* instanceMatrix * vec4(inPosition, 1.0);


    fragColor = inColor;
}


//layout(set=0, binding=1) 