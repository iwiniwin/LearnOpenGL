#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main(){
	// 对于万向阴影映射技术，是由几何着色器将顶点从世界空间变换到不同的光空间，所以顶点着色器只需要将顶点变换到世界空间即可
	gl_Position = model * vec4(aPos, 1.0);
}