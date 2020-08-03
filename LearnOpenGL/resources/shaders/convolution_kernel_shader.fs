#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D texture1;



void main(){
	const float offset = 1.0 / 300;  // 应该使用纹素大小

	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),  // 左上
		vec2(0, offset),  // 中上
		vec2(offset, offset),  // 右上
		vec2(-offset, 0),  // 左中
		vec2(0, 0),
		vec2(offset, 0),  // 右中
		vec2(-offset, -offset),  // 左下
		vec2(0, -offset),  // 中下
		vec2(offset, -offset)  // 右下
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