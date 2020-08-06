#version 330 core
layout (points) in;  // �����Ӷ�����ɫ�������ͼԪ����
layout (triangle_strip, max_vertices = 5) out;  // ָ��������ɫ�������ͼԪ���ͣ�ͬʱ�������������Ķ���������OpenGL��������ƶ���Ķ���

// �ڽ�������ע����һ������
// in gl_Vertex
// {
//	 vec4 gl_Position;
//	 float gl_PointSize;
//	 float gl_ClipDistance[];
// } gl_in[];

in VS_OUT{
	vec3 color;
} gs_in[];  // ��Ϊ������ɫ���������������һ�鶥�㣬���ԴӶ�����ɫ�������������������������ʽ��ʾ����

out vec3 fColor;  // Ϊ��һ��Ƭ����ɫ���׶�����һ�������ɫ����

void main(){
	fColor = gs_in[0].color;  // ������һ�������ʱ��ÿ�����㶼��ʹ��fColor�д����ֵ

	gl_Position = gl_in[0].gl_Position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();  // ÿ�ε���ʱ��gl_Position�е������ᱻ��ӵ�ͼԪ��

	gl_Position = gl_in[0].gl_Position + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();  // ÿ�ε���ʱ��gl_Position�е������ᱻ��ӵ�ͼԪ��

	gl_Position = gl_in[0].gl_Position + vec4(-0.2, 0.2, 0.0, 0.0);
	EmitVertex();  // ÿ�ε���ʱ��gl_Position�е������ᱻ��ӵ�ͼԪ��

	gl_Position = gl_in[0].gl_Position + vec4(0.2, 0.2, 0.0, 0.0);
	EmitVertex();  // ÿ�ε���ʱ��gl_Position�е������ᱻ��ӵ�ͼԪ��

	fColor = vec3(1.0f, 1.0f, 1.0f);  // �����һ���������ɫ����Ϊ��ɫ
	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.4, 0.0, 0.0);
	EmitVertex();  // ÿ�ε���ʱ��gl_Position�е������ᱻ��ӵ�ͼԪ��

	EndPrimitive();  // ������ʱ�����з����Ķ��㶼��ϳ�Ϊָ�������ͼԪ
}