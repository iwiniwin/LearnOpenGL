#version 330 core
layout (location = 0) out vec4 FragColor;  // 指定片段着色器写入到哪个颜色缓冲，前提是帧缓冲中绑定了多个颜色缓冲
layout (location = 1) out vec4 BrightColor;  // 指定片段着色器写入到哪个颜色缓冲，MRT，这样可以在一次渲染中就顺便把明亮区域提取了

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseMap;

struct Light {
	vec3 Position;
	vec3 Color;
};


uniform Light lights[16];
uniform vec3 viewPos;

void main(){
	vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;

	vec3 normal = normalize(fs_in.Normal);

	// ambient
	vec3 ambient = 0.0 * color;

	vec3 lighting = vec3(0.0);
	for(int i = 0; i < 4; i ++){
		// diffuse
		vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
		float diff = max(dot(lightDir, normal), 0.0f);
		vec3 diffuse = diff * color * lights[i].Color;

		vec3 result = diffuse;

		// attenuation
		float distance = length(fs_in.FragPos - lights[i].Position);
		result *= 1.0 / (distance * distance);  // 因为会在后处理做gamma校正，所以这里使用平方
		// result *= 1.0 / distance;

		lighting += result;
	}
	// 正常的光照计算
	FragColor = vec4(lighting + ambient, 1.0f);

	// 提取明亮颜色
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0){
		BrightColor = vec4(FragColor.rgb, 1.0f);
	}else{
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);  // 注意else的时候也要设置颜色
	}
}