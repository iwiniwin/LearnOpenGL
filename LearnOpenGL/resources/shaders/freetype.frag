#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main(){
	// 使用单颜色通道位图纹理中的红色分量作为alpha值
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	color = vec4(textColor, 1.0) * sampled;
}