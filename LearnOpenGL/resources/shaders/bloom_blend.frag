#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D blur;

uniform bool bloom;

uniform float exposure;

void main(){
	const float gamma = 2.2;
	vec3 sceneColor = texture(scene, TexCoords).rgb;
	vec3 blurColor = texture(blur, TexCoords).rgb;

	if(bloom)  // 是否开启bloom
		sceneColor += blurColor;  // additive blending

	// 色调映射
	vec3 result = vec3(1.0) - exp(-sceneColor * exposure);
	result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0f);
}