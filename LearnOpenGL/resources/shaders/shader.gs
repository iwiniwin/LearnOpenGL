#version 330 core
layout (points) in;  // 申明从顶点着色器输入的图元类型
layout (triangle_strip, max_vertices = 5) out;  // 指定几何着色器输出的图元类型，同时设置最大能输出的顶点数量，OpenGL将不会绘制多出的顶点

// 内建变量，注意是一个数组
// in gl_Vertex
// {
//	 vec4 gl_Position;
//	 float gl_PointSize;
//	 float gl_ClipDistance[];
// } gl_in[];

in VS_OUT{
	vec3 color;
} gs_in[];  // 因为几何着色器是作用于输入的一组顶点，所以从顶点着色器发来的数据总是以数组的形式表示出来

out vec3 fColor;  // 为下一个片段着色器阶段声明一个输出颜色向量

void main(){
	fColor = gs_in[0].color;  // 当发射一个顶点的时候，每个顶点都会使用fColor中储存的值

	gl_Position = gl_in[0].gl_Position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();  // 每次调用时，gl_Position中的向量会被添加到图元中

	gl_Position = gl_in[0].gl_Position + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();  // 每次调用时，gl_Position中的向量会被添加到图元中

	gl_Position = gl_in[0].gl_Position + vec4(-0.2, 0.2, 0.0, 0.0);
	EmitVertex();  // 每次调用时，gl_Position中的向量会被添加到图元中

	gl_Position = gl_in[0].gl_Position + vec4(0.2, 0.2, 0.0, 0.0);
	EmitVertex();  // 每次调用时，gl_Position中的向量会被添加到图元中

	fColor = vec3(1.0f, 1.0f, 1.0f);  // 将最后一个顶点的颜色设置为白色
	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.4, 0.0, 0.0);
	EmitVertex();  // 每次调用时，gl_Position中的向量会被添加到图元中

	EndPrimitive();  // 被调用时，所有发出的顶点都会合成为指定的输出图元
}