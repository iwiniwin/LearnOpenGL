#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexDirection;

uniform mat4 view;
uniform mat4 projection;

void main(){
	// 注意 乘法要从右往左读
	vec4 pos = projection * view * vec4(aPos, 1.0);
	// 使天空盒的深度为1
	gl_Position = pos.xyzz;
	TexDirection = aPos;
}