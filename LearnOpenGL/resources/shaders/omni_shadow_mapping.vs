#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main(){
	// ����������Ӱӳ�似�������ɼ�����ɫ�������������ռ�任����ͬ�Ĺ�ռ䣬���Զ�����ɫ��ֻ��Ҫ������任������ռ伴��
	gl_Position = model * vec4(aPos, 1.0);
}