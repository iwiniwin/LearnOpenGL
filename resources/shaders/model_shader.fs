#version 330 core

in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoords;

out vec4 FragColor;

// 材质
struct Material {
	// vec3 ambient;
	float shininess;
	sampler2D texture_diffuse1;
    sampler2D texture_specular1;
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
// 点光源数量
#define NR_POINT_LIGHTS 2


uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform vec3 viewPos;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(light.position - WorldPos);
	// 漫反射颜色
	vec3 diffuse = max(0, dot(lightDir, normal)) * light.diffuse * texture(material.texture_diffuse1, TexCoords).rgb;
	// 镜面光颜色
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(0, dot(reflectDir, viewDir)), material.shininess) * light.specular * texture(material.texture_specular1, TexCoords).rgb;
	vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

	float distance = length(light.position - WorldPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	return ambient * attenuation + diffuse * attenuation + specular * attenuation;
}

void main(){

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - WorldPos);
    vec3 outColor = vec3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < NR_POINT_LIGHTS; i ++){
       outColor += calcPointLight(pointLights[i], norm, viewDir);
    }
   FragColor = vec4(outColor, 1.0f);
}