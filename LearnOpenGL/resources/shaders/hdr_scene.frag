#version 330 core

out vec4 FragColor;

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
	for(int i = 0; i < 16; i ++){
		// diffuse
		vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
		float diff = max(dot(lightDir, normal), 0.0f);
		vec3 diffuse = diff * color * lights[i].Color;

		vec3 result = diffuse;

		// attenuation
		float distance = length(fs_in.FragPos - lights[i].Position);
		result *= 1.0 / (distance * distance);  // 因为会在后处理做gamma校正，所以这里使用平方
//		result *= 1.0 / distance;

		lighting += result;
	}
	FragColor = vec4(lighting + ambient, 1.0f);
}