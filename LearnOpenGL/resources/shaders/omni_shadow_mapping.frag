#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main(){
	// 自己计算深度值

	// 得到片元和光源之间的距离
	float lightDistance = length(FragPos.xyz - lightPos);
	// 将距离映射到[0, 1]
	lightDistance /= far_plane;

	gl_FragDepth = lightDistance;
}