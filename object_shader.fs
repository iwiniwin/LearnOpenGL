#version 330 core

in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoords;

out vec4 FragColor;

// ����
struct Material {
	// vec3 ambient;
	// vec3 diffuse;  // ��������ɫ�ڼ�����������¶�������������ɫ
	sampler2D diffuse;  // ��������ͼ
	vec3 specular;
	float shininess;
};

// �������
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
	// reflect����Ҫ���һ�������Ǵӹ�Դ����ָ��Ƭ��λ�õģ���������ȡ��
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32�Ǹ߹�ķ���ȣ�һ������ķ����Խ�󣬷���������Խǿ���߹���ԽС
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess) * material.specular * light.specular;

	FragColor = vec4((ambient + diffuse + specular), 1.0f);  // ���ĸ����������ó�1.0
}