#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out LAST_OUT{
	vec2 texCoords;
} vs_out;

uniform mat4 view;
uniform mat4 projection;

void main(){
	// 注意 乘法要从右往左读
	gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
	vs_out.texCoords = aTexCoords;
}