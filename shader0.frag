#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable



//Output
layout(location = 0) out vec4 outColor  ;


layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput RinputColor;
layout (input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput GinputColor;
layout (input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput DepthValue;




void main() {

//    debugPrintfEXT("x");
//    debugPrintfEXT("%f", 1.0);
  
//    outColor = texture(texSampler, fragTexCoord);
//  
//    RoutColor=outColor;
//    GoutColor=outColor;
//

  
    // Read color from previous color input attachment
    vec3 color = subpassLoad(RinputColor).rgb;
    outColor.rgb = brightnessContrast(color, ubo.brightnessContrast[0], ubo.brightnessContrast[1]);
    


    // Read depth from previous depth input attachment
    float depth = subpassLoad(inputDepth).r;
    outColor.rgb = vec3((depth - ubo.range[0]) * 1.0 / (ubo.range[1] - ubo.range[0]));


}