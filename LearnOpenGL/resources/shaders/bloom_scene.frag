#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;

void main(){
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	vec3 mapped = hdrColor;

	// Reinhard色调映射
	mapped = hdrColor / (hdrColor + vec3(1.0));

	// 曝光色调映射
	float exposure = 0.1;  // 曝光度
	mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// gamma校正
	mapped = pow(mapped, vec3(1/2.2));

	FragColor = vec4(mapped, 1.0f);
}