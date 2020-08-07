#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT{
	vec3 clipSpaceNormal;
} gs_in[];

out vec3 fColor; 

void generateLine(vec4 position, vec3 normal){
	gl_Position = position;
	EmitVertex();

	float magnitude = 0.4f;
	position += vec4(normal * magnitude, 0.0);
	gl_Position = position;
	EmitVertex();

	EndPrimitive();
}

void main(){
	fColor = vec3(1.0f, 1.0f, 0.0f);
	generateLine(gl_in[0].gl_Position, gs_in[0].clipSpaceNormal);
	generateLine(gl_in[1].gl_Position, gs_in[1].clipSpaceNormal);
	generateLine(gl_in[2].gl_Position, gs_in[2].clipSpaceNormal);
}