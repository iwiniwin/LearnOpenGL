#version 330 core

in vec2 TexCoords;

out vec4 FragColor;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main(){
	if(gl_FragCoord.x < 400)
		FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else
		FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	
	if(gl_FrontFacing)
		FragColor = texture(texture1, TexCoords);
	else
		FragColor = texture(texture2, TexCoords);
}