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
	// vec3 specular;
	sampler2D specular;
	float shininess;
};

// �������
struct Light {
	vec3 position;
	vec3 direction;  // �۹ⷽ��
	float cutoff;  // ���й��
	float outercutoff;  // ���й��
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

	float theta = dot(lightDir, normalize(-light.direction));
	float intensity = clamp((theta - light.outercutoff) / (light.cutoff - light.outercutoff), 0.0, 1.0);

	vec3 diffuse = max(0, dot(lightDir, norm)) * light.diffuse * vec3(texture(material.diffuse, TexCoords));

	vec3 viewDir = normalize(viewPos - WorldPos);
	// reflect����Ҫ���һ�������Ǵӹ�Դ����ָ��Ƭ��λ�õģ���������ȡ��
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32�Ǹ߹�ķ���ȣ�һ������ķ����Խ�󣬷���������Խǿ���߹���ԽС
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess) * texture(material.specular, TexCoords).rgb * light.specular;

	FragColor = vec4((ambient + diffuse * intensity + specular * intensity), 1.0f);  // ���ĸ����������ó�1.0

	
}