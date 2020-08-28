#version 330 core

out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	mat3 TBN;
} fs_in;

in VS_TANGENT_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 LightPos;
	vec3 ViewPos;
	vec3 TangentFragPos;
} fs_tangent_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

// 在世界空间中计算光照
void main(){
	vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;

	vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(fs_in.TBN * normal);

//	vec3 normal = normalize(fs_in.Normal);

	vec3 lightColor = vec3(0.6);

	// ambient
	vec3 ambient = 0.1 * color;

	// diffuse
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * color;

	// specular
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(halfwayDir, normal), 0.0f), 64.0f);
	vec3 specular = spec * lightColor;

	// shaow
	vec3 lighting = (ambient + diffuse + specular) * color;
	FragColor = vec4(lighting, 1.0f);
}

// 在切线空间计算光照
//void main(){
//	vec3 color = texture(diffuseMap, fs_tangent_in.TexCoords).rgb;
//
//	vec3 normal = texture(normalMap, fs_tangent_in.TexCoords).rgb;
//	normal = normalize(normal * 2.0 - 1.0);
//
////	vec3 normal = normalize(fs_in.Normal);
//
//	vec3 lightColor = vec3(0.3);
//
//	// ambient
//	vec3 ambient = 0.1 * color;
//
//	// diffuse
//	vec3 lightDir = normalize(fs_tangent_in.LightPos - fs_tangent_in.TangentFragPos);
//	float diff = max(dot(lightDir, normal), 0.0f);
//	vec3 diffuse = diff * color;
//
//	// specular
//	vec3 viewDir = normalize(fs_tangent_in.ViewPos - fs_tangent_in.TangentFragPos);
//	float spec = 0.0;
//	vec3 halfwayDir = normalize(lightDir + viewDir);
//	spec = pow(max(dot(halfwayDir, normal), 0.0f), 64.0f);
//	vec3 specular = spec * lightColor;
//
//	// shaow
//	vec3 lighting = (ambient + diffuse + specular) * color;
//	FragColor = vec4(lighting, 1.0f);
//}