#version 330 core

in vec2 TexCoords;
layout (depth_greater) out float gl_FragDepth;
out vec4 FragColor;
uniform sampler2D texture1;
uniform sampler2D texture2;

in VS_OUT{  // 块名必须和顶点着色器的相同
    vec2 TexCoords;  
} fs_in;  // 实例名不要求与顶点着色器的相同

void main(){
	if(gl_FragCoord.x < 400)
		FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else
		FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	
	if(gl_FrontFacing)
		FragColor = texture(texture1, TexCoords);
	else
		FragColor = texture(texture2, fs_in.TexCoords);
    // gl_FragDepth = 0.1;
}