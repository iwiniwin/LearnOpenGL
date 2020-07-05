#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 WorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	// 注意 乘法要从右往左读
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// 使用3x3矩阵，避免产生位移影响
	Normal = mat3(transpose(inverse(model))) * aNormal;
	WorldPos = vec3(model * vec4(aPos, 1.0));
}