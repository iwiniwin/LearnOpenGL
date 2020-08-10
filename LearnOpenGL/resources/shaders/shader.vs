#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 WorldPos;
out vec2 TexCoords;

out VS_OUT{
	vec3 clipSpaceNormal;
} vs_out;

out LAST_OUT{
	vec2 texCoords;
} vs_out2;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fColor;

void main(){
	// 注意 乘法要从右往左读
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// 使用3x3矩阵，避免产生位移影响
	Normal = mat3(transpose(inverse(model))) * aNormal;
	WorldPos = vec3(model * vec4(aPos, 1.0));
	TexCoords = aTexCoords;

	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vs_out.clipSpaceNormal = normalize(vec3(projection * vec4(normalMatrix * Normal, 1.0)));
	vs_out2.texCoords = aTexCoords;
}