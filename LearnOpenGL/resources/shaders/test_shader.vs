#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT{  // 块名必须和片段着色器的相同
	vec2 TexCoords;
} vs_out;  // 实例名不要求与片段着色器的相同

out vec2 TexCoords;

void main(){
	// 注意 乘法要从右往左读
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// gl_PointSize = gl_Position.z;
	// gl_PointSize = 30;
	TexCoords = aTexCoords;
	vs_out.TexCoords = aTexCoords;
}