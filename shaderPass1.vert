#version 460
#extension GL_ARB_separate_shader_objects : enable

//#extension GL_KHR_vulkan_glsl : enable

#extension GL_EXT_debug_printf : enable






//Input
layout(location = 0) in vec2 inPostion;


void main() 
{

//        debugPrintfEXT("Here are two float values \n");
//
//       debugPrintfEXT("Here's a smaller float value \n");
//
//      debugPrintfEXT("Here's an float %f with text before and after it\n",  uboShit.model[0][0]);


	gl_Position = vec4(inPostion,0.f,1.f);


  
   
}


//layout(set=0, binding=1) 