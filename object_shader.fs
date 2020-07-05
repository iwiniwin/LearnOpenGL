#version 330 core

in vec3 Normal;
in vec3 WorldPos;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main(){
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - WorldPos);
	vec3 diffuse = max(0, dot(lightDir, norm)) * lightColor;

	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - WorldPos);
	// reflect����Ҫ���һ�������Ǵӹ�Դ����ָ��Ƭ��λ�õģ���������ȡ��
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32�Ǹ߹�ķ���ȣ�һ������ķ����Խ�󣬷���������Խǿ���߹���ԽС
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0f), 32) * specularStrength * lightColor;

	FragColor = vec4(objectColor * (ambient + diffuse + specular), 1.0f);  // ���ĸ����������ó�1.0
}