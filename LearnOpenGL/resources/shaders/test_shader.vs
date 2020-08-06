#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform Matrices{  // һ����Matrices��Uniform�飬Uniform���еı�������ֱ�ӷ��ʣ�����Ҫ�ӿ�����Ϊǰ׺
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out VS_OUT{  // ���������Ƭ����ɫ������ͬ
	vec2 TexCoords;
} vs_out;  // ʵ������Ҫ����Ƭ����ɫ������ͬ

out vec2 TexCoords;

void main(){
	// ע�� �˷�Ҫ���������
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// gl_PointSize = gl_Position.z;
	// gl_PointSize = 30;
	TexCoords = aTexCoords;
	vs_out.TexCoords = aTexCoords;
}