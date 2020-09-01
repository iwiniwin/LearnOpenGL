#version 330 core
layout (location = 0) out vec3 gPosition;  // 指定片段着色器写入到哪个颜色缓冲，前提是帧缓冲中绑定了多个颜色缓冲
layout (location = 1) out vec3 gNormal;  // 指定片段着色器写入到哪个颜色缓冲，MRT
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main(){
	// 存储第一个G缓冲纹理中的片段位置向量
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
	// 存储镜面强度到gAlbedoSpec的alpha分量
	gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}