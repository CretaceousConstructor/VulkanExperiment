#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

layout(set = 0,binding = 0) uniform  ShaderDataUBO{

	mat4 view;
	mat4 proj;
	vec4 lightPos;
	vec3 lightColor;
	vec3 eyePos;
	float ambientStrength;
	float specularStrength;

} ubo_scene;


////uniform 
layout (set = 1, binding = 0) uniform sampler2D samplerColorMap;

////Input
layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inWoldPos;

////Output
layout(location = 0) out vec4 outFragColor;




void main() {




	 	vec4 objectColor = texture(samplerColorMap, inUV) * vec4(inColor, 1.0); //得到texutre中的颜色后，乘以baseColorFactor

		vec3 norm  = normalize(inNormal);
		vec3 lightDir  = normalize(vec3(ubo_scene.lightPos) - inWoldPos);
		vec3 viewDir = normalize(ubo_scene.eyePos - inWoldPos);
		vec3 reflectDir = reflect(-lightDir, norm);  

		vec3 ambient = ubo_scene.ambientStrength * ubo_scene.lightColor;
			
		
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * ubo_scene.lightColor;
	
	
		vec3 specular = vec3(0.); 
		if( dot(norm,lightDir) > 0.){
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
			specular = ubo_scene.specularStrength * spec * ubo_scene.lightColor; 
		}

		vec3 result = (ambient + diffuse + specular ) * vec3(objectColor);

		outFragColor = vec4(result, 1.0);



  

}