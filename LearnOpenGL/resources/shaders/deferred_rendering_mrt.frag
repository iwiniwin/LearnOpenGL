#version 330 core
layout (location = 0) out vec3 gPosition;  // ָ��Ƭ����ɫ��д�뵽�ĸ���ɫ���壬ǰ����֡�����а��˶����ɫ����
layout (location = 1) out vec3 gNormal;  // ָ��Ƭ����ɫ��д�뵽�ĸ���ɫ���壬MRT
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main(){
	// �洢��һ��G���������е�Ƭ��λ������
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
	// �洢����ǿ�ȵ�gAlbedoSpec��alpha����
	gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}