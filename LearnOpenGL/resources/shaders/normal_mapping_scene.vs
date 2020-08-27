#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 fColor;

uniform bool reverse_normals;

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	if(reverse_normals)
		vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0f * aNormal);
	else
		vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	vs_out.TexCoords = aTexCoords;
}