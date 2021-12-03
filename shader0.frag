#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable


//
//
layout (input_attachment_index = 0, binding = 0) uniform subpassInput inputRColor;
layout (input_attachment_index = 1, binding = 1) uniform subpassInput inputDepth;
//
//
//
////Output
layout(location = 0) out vec4 outColor;

void main() {
//
////    debugPrintfEXT("x");
////    debugPrintfEXT("%f", 1.0);


		// Read color from previous color input attachment
		outColor.rgb = subpassLoad(inputRColor).rgb;




}