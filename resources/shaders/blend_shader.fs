#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
uniform sampler2D texture1;


void main(){
	vec4 textureColor = texture(texture1, TexCoords);

	// ͸���Ȳ��ԣ�����Ƭ��
	// if(textureColor.a < 0.1)
	//	discard;
	// FragColor = textureColor;

	// ���
	FragColor = textureColor;
}