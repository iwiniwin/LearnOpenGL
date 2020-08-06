#version 330 core

out vec4 FragColor;

in vec3 fColor;

void main(){
	FragColor = vec4(0.04, 0.28, 0.26, 1.0) + vec4(fColor, 0.0f);
}