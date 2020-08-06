#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float time;

in vec2 TexCoords[];

out LAST_OUT{  
	vec2 texCoords;
} gs_out;

vec4 explode(vec4 position, vec3 normal){
	float magnitude = 2.0;
	// 这里sin值是-1到1，转换到0到1
	return position += vec4((sin(time) + 1) / 2.0 * magnitude * normal, 0.0);
}

void main(){
	vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	vec3 normal = normalize(cross(a, b));

	gl_Position = explode(gl_in[0].gl_Position, normal);
	gs_out.texCoords = TexCoords[0];  // 需要帮忙透传顶点着色器的纹理坐标到片段着色器
	EmitVertex();

	gl_Position = explode(gl_in[1].gl_Position, normal);
	gs_out.texCoords = TexCoords[1];
	EmitVertex();

	gl_Position = explode(gl_in[2].gl_Position, normal);
	gs_out.texCoords = TexCoords[2];
	EmitVertex();

	EndPrimitive();
}