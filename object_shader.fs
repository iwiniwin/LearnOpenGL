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
	// reflect函数要求第一个向量是从光源方向指向片段位置的，所以这里取反
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32是高光的反光度，一个物体的反光度越大，反射光的能力越强，高光点会越小
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0f), 32) * specularStrength * lightColor;

	FragColor = vec4(objectColor * (ambient + diffuse + specular), 1.0f);  // 将四个分量都设置成1.0
}