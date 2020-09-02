#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct Light {
	vec3 Position;
	vec3 Color;

	float Linear;
	float Quadratic;
};

uniform vec3 viewPos;

uniform Light light;

void main(){
	
	// 从G缓冲中获取数据
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedo, TexCoords).rgb;

	float ambientOcclusion = texture(ssao, TexCoords).r;

	vec3 ambient = vec3(0.3 * Albedo);

	vec3 lighting = ambient;

	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 lightDir = normalize(light.Position - FragPos);
	float diff = max(dot(lightDir, Normal), 0.0f);
	vec3 diffuse = diff * Albedo * light.Color;

	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, Normal), 0.0), 8.0);
	vec3 specular = light.Color * spec;

	// attenuation
	float distance = length(FragPos - light.Position);
	float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
		
	diffuse *= attenuation;
	specular *= attenuation;

	lighting += diffuse + specular;

	FragColor = vec4(lighting, 1.0f);
}