#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 Position;
	vec3 Color;

	float Linear;
	float Quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform vec3 viewPos;

void main(){
	
	// 从G缓冲中获取数据
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	// 和往常一样计算光照
	vec3 lighting = Albedo * 0.1;  // 环境光照分量
	vec3 viewDir = normalize(viewPos - FragPos);
	for(int i = 0; i < NR_LIGHTS; i ++){
		// diffuse
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		float diff = max(dot(lightDir, Normal), 0.0f);
		vec3 diffuse = diff * Albedo * lights[i].Color;

		// specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(halfwayDir, Normal), 0.0), 16.0);
		vec3 specular = lights[i].Color * spec * Specular;

		// attenuation
		float distance = length(FragPos - lights[i].Position);
		float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
		
		diffuse *= attenuation;
		specular *= attenuation;

		lighting += diffuse + specular;
	}
	FragColor = vec4(lighting, 1.0f);
}