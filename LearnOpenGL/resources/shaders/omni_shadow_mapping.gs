#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;  

void main(){
	
	// 输入一个三角形，输出6个三角形

	for(int face = 0; face < 6; face ++){
		gl_Layer = face;  // 内置变量，指定发射出的基本图元要送到立方体贴图的哪一个面

		// 发送一个三角形
		for(int i = 0; i < 3; i ++){
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos;  // 将每个世界空间顶点变换到光空间
			EmitVertex();
		}
		EndPrimitive();
	}
}