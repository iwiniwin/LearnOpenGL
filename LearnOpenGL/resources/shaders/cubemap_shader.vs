#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexDirection;

uniform mat4 view;
uniform mat4 projection;

void main(){
	// ע�� �˷�Ҫ���������
	vec4 pos = projection * view * vec4(aPos, 1.0);
	// ʹ��պе����Ϊ1
	gl_Position = pos.xyzz;
	TexDirection = aPos;
}