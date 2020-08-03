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
};

// 定向光
struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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


// 聚光灯
struct SpotLight {
	vec3 position;
	vec3 direction;  // 聚光方向
	float cutoff;  // 内切光角
	float outercutoff;  // 外切光角
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);
	// 漫反射颜色
	vec3 diffuse = max(0, dot(lightDir, normal)) * light.diffuse * texture(material.diffuse, TexCoords).rgb;
	// 镜面光颜色
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(0, dot(reflectDir, viewDir)), material.shininess) * light.specular * texture(material.specular, TexCoords).rgb;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	return ambient + diffuse + specular;
}

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

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(light.position - WorldPos);
	// 漫反射颜色
	vec3 diffuse = max(0, dot(lightDir, normal)) * light.diffuse * texture(material.diffuse, TexCoords).rgb;
	// 镜面光颜色
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(0, dot(reflectDir, viewDir)), material.shininess) * light.specular * texture(material.specular, TexCoords).rgb;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	float distance = length(light.position - WorldPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	float theta = dot(lightDir, normalize(-light.direction));
	float intensity = clamp((theta - light.outercutoff) / (light.cutoff - light.outercutoff), 0.0, 1.0);

	return ambient * attenuation * intensity + diffuse * attenuation * intensity + specular * attenuation * intensity;
}

void main(){

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - WorldPos);

	// 定向光照
	vec3 result = calcDirLight(dirLight, norm, viewDir);
	// 点光源
	for(int i = 0; i < NR_POINT_LIGHTS; i ++){
		result += calcPointLight(pointLights[i], norm, viewDir);
	}
	// 聚光灯
	result += calcSpotLight(spotLight, norm, viewDir);
	FragColor = vec4(result, 1.0f);
}