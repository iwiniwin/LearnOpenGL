#version 330 core

out vec4 FragColor;

in vec3 TexDirection;
uniform samplerCube cubemap;

void main(){
	FragColor = texture(cubemap, TexDirection);
}