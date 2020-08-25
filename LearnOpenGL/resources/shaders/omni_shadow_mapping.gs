#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;  

void main(){
	
	// ����һ�������Σ����6��������

	for(int face = 0; face < 6; face ++){
		gl_Layer = face;  // ���ñ�����ָ��������Ļ���ͼԪҪ�͵���������ͼ����һ����

		// ����һ��������
		for(int i = 0; i < 3; i ++){
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos;  // ��ÿ������ռ䶥��任����ռ�
			EmitVertex();
		}
		EndPrimitive();
	}
}