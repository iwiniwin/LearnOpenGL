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
	vec3 specular;
	float shininess;
};

// 光的属性
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main(){
	vec3 ambient = texture(material.diffuse, TexCoords).rgb * light.ambient;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - WorldPos);
	vec3 diffuse = max(0, dot(lightDir, norm)) * light.diffuse * vec3(texture(material.diffuse, TexCoords));

	vec3 viewDir = normalize(viewPos - WorldPos);
	// reflect函数要求第一个向量是从光源方向指向片段位置的，所以这里取反
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32是高光的反光度，一个物体的反光度越大，反射光的能力越强，高光点会越小
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess) * material.specular * light.specular;

	FragColor = vec4((ambient + diffuse + specular), 1.0f);  // 将四个分量都设置成1.0
}