#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D texture1;



void main(){
	const float offset = 1.0 / 300;  // Ӧ��ʹ�����ش�С

	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),  // ����
		vec2(0, offset),  // ����
		vec2(offset, offset),  // ����
		vec2(-offset, 0),  // ����
		vec2(0, 0),
		vec2(offset, 0),  // ����
		vec2(-offset, -offset),  // ����
		vec2(0, -offset),  // ����
		vec2(offset, -offset)  // ����
	);

float kernel[9] = float[](
	1, 1, 1,
	1, -8, 1,
	1, 1, 1
);

	vec3 col = vec3(0.0f);
	for(int i = 0; i < 9; i ++){
		col += vec3(texture(texture1, TexCoords.st + offsets[i])) * kernel[i];
	}
	FragColor = vec4(col, 1.0f);
}