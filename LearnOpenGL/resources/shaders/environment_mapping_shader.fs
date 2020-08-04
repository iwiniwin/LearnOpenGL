#version 330 core

out vec4 FragColor;

uniform vec3 cameraPos;

in vec3 WorldPos;
in vec3 Normal;
uniform samplerCube cubemap;

void main(){
    vec3 I = normalize(WorldPos - cameraPos);
	// ∑¥…‰
    vec3 R = reflect(I, normalize(Normal));
	// ’€…‰
	// vec3 R = refract(I, normalize(Normal), 1 / 1.52);
	FragColor = vec4(texture(cubemap, R).rgb, 1.0);
}