#version 330 core

out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

float shadowCalculation(vec3 fragPos){
	vec3 fragToLight = fragPos - lightPos;  // ʹ�����������Ϊ������������������ͼ���в���
	float closestDepth = texture(depthMap, fragToLight).r;
	closestDepth *= far_plane;
	float currentDepth = length(fragToLight);

	float bias = 0.05;
//	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// pcf
	float shadow = 0.0f;
	
	float offset = 0.1;

	// ��ԭʼ������������������������64
//	float samples = 4.0;
//	for(float x = -offset; x < offset; x += offset / (samples * 0.5)){
//		for(float y = -offset; y < offset; y += offset / (samples * 0.5)){
//			for(float z = -offset; z < offset; z += offset / (samples * 0.5)){
//				float closetDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r;
//				closetDepth *= far_plane;
//				shadow += (currentDepth - bias > closetDepth ? 1.0 : 0.0);
//			}
//		}
//	}
//	shadow /= (samples * samples * samples);
	
	// ʹ����ԭʼ����������ֱ�ķ�����в�����������20
	vec3 sampleOffsetDirections[20] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);
	float samples = 20;
	float viewDistance = length(fragPos - viewPos);
//	float diskRadius = 0.05;
	
	for(int i = 0; i < samples; i ++){
		float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;  // ���ݹ۲��ߵľ�������뾶���������Զʱ����Ӱ����͡�����ʱ��Ӱ������
		float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;
		shadow += (currentDepth - bias > closestDepth ? 1.0 : 0.0);
	}
	shadow /= samples;

	return shadow;
}

void main(){
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(0.3);

	// ambient
	vec3 ambient = 0.2 * color;

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
	float shadow = shadowCalculation(fs_in.FragPos);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	FragColor = vec4(lighting, 1.0f);
}