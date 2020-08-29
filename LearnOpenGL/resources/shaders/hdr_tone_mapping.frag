#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;

void main(){
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	vec3 mapped = hdrColor;

	// Reinhardɫ��ӳ��
	mapped = hdrColor / (hdrColor + vec3(1.0));

	// �ع�ɫ��ӳ��
	float exposure = 0.1;  // �ع��
	mapped = vec3(1.0) - exp(-hdrColor * exposure);

	// gammaУ��
	mapped = pow(mapped, vec3(1/2.2));

	FragColor = vec4(mapped, 1.0f);
}