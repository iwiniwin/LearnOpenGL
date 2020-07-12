#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main(){
	// texture函数会根据设置的纹理参数对相应的颜色值进行采样
	// FragColor = texture(texture2, texCoord) * vec4(color, 1.0f);
	// mix函数对两个颜色进行混合，线性插值，第三个参数0.2，可以表示第二个颜色在混合中所占的比例
	FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
}