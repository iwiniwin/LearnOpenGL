#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;


uniform sampler2D texture1;
uniform sampler2D depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir){
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 由于深度贴图值是在0到1之间，而projCoords是在-1到1之间，所以需要转换到相同范围
    projCoords = projCoords * 0.5 + 0.5;
    
    
	// float shadow = projCoords.z > closestDepth ? 1 : 0;

	// 阴影偏移

	float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.005);

//	bias = 0;
	
//	bias = 0.005;

	if(projCoords.z > 1.0){
		return 0;
	}

//	float closestDepth = texture(depthMap, projCoords.xy).r;

	// pcf
	float closestDepth = 0;
	float shadow = 0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; x ++){
		for(int y = -1; y <= 1; y ++){
			closestDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += projCoords.z - bias > closestDepth ? 1 : 0;
		}
	}


	shadow /= 9.0;
	
    return shadow;
}

void main(){
	vec3 color = texture(texture1, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    // ambient
    vec3 ambient = 0.15 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(0, dot(lightDir, normal));
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(0, dot(halfDir, normal)), 64.0);
    vec3 specular = spec * lightColor;

    // shadow
    float shadow = calculateShadow(fs_in.FragPosLightSpace, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    FragColor = vec4(lighting, 1.0);
}