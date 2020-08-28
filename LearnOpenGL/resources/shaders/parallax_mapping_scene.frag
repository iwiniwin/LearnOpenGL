#version 330 core

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec3 TangentNormal;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D dispMap;

uniform float heightScale;

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir){
	float height = texture(dispMap, texCoords).r;
	// 这里除以viewDir.z是因为，当viewDir大致平行于表面时，它的z坐标接近于0，除法会返回更大的p向量。
	// 从本质上理解就是，相比于垂直看向平面，当带有角度看向平面时，会更大程度地缩放p向量，从而增肌纹理坐标的偏移，在视角上会获得更大的真实度
	vec2 p = viewDir.xy / viewDir.z * (height * heightScale);  
//	vec2 p = viewDir.xy * (height * heightScale);
	return texCoords - p;
}

void main(){
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	vec2 texCoords = parallaxMapping(fs_in.TexCoords, viewDir);

	vec3 color = texture(diffuseMap, texCoords).rgb;

	vec3 normal = texture(normalMap, texCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

//	vec3 normal = normalize(fs_in.TangentNormal);

	vec3 lightColor = vec3(0.3);

	// ambient
	vec3 ambient = 0.1 * color;

	// diffuse
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * color;

	// specular
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(halfwayDir, normal), 0.0f), 64.0f);
	vec3 specular = spec * lightColor;

	// shaow
	vec3 lighting = (ambient + diffuse + specular) * color;
	FragColor = vec4(lighting, 1.0f);
}