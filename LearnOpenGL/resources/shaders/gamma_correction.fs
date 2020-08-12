#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture1;


void main(){
	float gamma = 2.2;
	gamma = 1.0;
	FragColor = texture(texture1, TexCoords);
	FragColor.rgb = pow(FragColor.rgb, vec3(1/gamma));
}