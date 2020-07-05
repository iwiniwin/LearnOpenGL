#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 WorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	// ע�� �˷�Ҫ���������
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// ʹ��3x3���󣬱������λ��Ӱ��
	Normal = mat3(transpose(inverse(model))) * aNormal;
	WorldPos = vec3(model * vec4(aPos, 1.0));
}