#version 330 core

in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoords;

out vec4 FragColor;

// 材质
struct Material {
	// vec3 ambient;
	// vec3 diffuse;  // 环境光颜色在几乎所有情况下都等于漫反射颜色
	sampler2D diffuse;  // 漫反射贴图
	// vec3 specular;
	sampler2D specular;
	float shininess;
	sampler2D texture_diffuse1;
};

// 点光源
struct PointLight {
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
// 4 个点光源
#define NR_POINT_LIGHTS 4


uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform vec3 viewPos;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(light.position - WorldPos);
	// 漫反射颜色
	vec3 diffuse = max(0, dot(lightDir, normal)) * light.diffuse * texture(material.diffuse, TexCoords).rgb;
	// 镜面光颜色
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(0, dot(reflectDir, viewDir)), material.shininess) * light.specular * texture(material.specular, TexCoords).rgb;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	float distance = length(light.position - WorldPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	return ambient * attenuation + diffuse * attenuation + specular * attenuation;
}

void main(){

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - WorldPos);
	// vec3 lightDir = normalize(light.position - WorldPos);
	
	// vec3 diffuse = max(0, dot(lightDir, norm)) * light.diffuse * texture(material.texture_diffuse1, TexCoords).rgb;
	// FragColor = vec4(diffuse, 1.0f);
	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}