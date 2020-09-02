#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool inverse_normals;

void main(){

	gl_Position = projection * view * model * vec4(aPos, 1.0);

	// ��Ϊ���ֵ�ǹ۲�ռ�ģ���������ʸ��Ҳ��Ҫת�����۲�ռ�

	FragPos = vec3(view * model * vec4(aPos, 1.0f));
	TexCoords = aTexCoords;

	vec3 n = inverse_normals ? -aNormal : aNormal;

	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	Normal = normalize(normalMatrix * n);
}