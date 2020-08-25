#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main(){
	// �Լ��������ֵ

	// �õ�ƬԪ�͹�Դ֮��ľ���
	float lightDistance = length(FragPos.xyz - lightPos);
	// ������ӳ�䵽[0, 1]
	lightDistance /= far_plane;

	gl_FragDepth = lightDistance;
}