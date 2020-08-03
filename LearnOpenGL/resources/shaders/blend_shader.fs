#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
uniform sampler2D texture1;


void main(){
	vec4 textureColor = texture(texture1, TexCoords);

	// Í¸Ã÷¶È²âÊÔ£¬¶ªÆúÆ¬¶Î
	// if(textureColor.a < 0.1)
	//	discard;
	// FragColor = textureColor;

	// »ìºÏ
	FragColor = textureColor;
}