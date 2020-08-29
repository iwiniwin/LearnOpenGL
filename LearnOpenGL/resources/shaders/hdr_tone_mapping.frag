#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;

void main(){
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	vec3 mapped = hdrColor;

	// Reinhard色调映射
	mapped = hdrColor / (hdrColor + vec3(1.0));

	// gamma校正
	mapped = pow(mapped, vec3(1/2.2));

	FragColor = vec4(mapped, 1.0f);
}