#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main(){
	// texture������������õ������������Ӧ����ɫֵ���в���
	// FragColor = texture(texture2, texCoord) * vec4(color, 1.0f);
	// mix������������ɫ���л�ϣ����Բ�ֵ������������0.2�����Ա�ʾ�ڶ�����ɫ�ڻ������ռ�ı���
	FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
}