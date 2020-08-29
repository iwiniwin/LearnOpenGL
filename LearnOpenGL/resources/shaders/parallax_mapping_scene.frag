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

// ��ͨ�Ӳ�ӳ��
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir){
	float height = texture(dispMap, texCoords).r;
	// �������viewDir.z����Ϊ����viewDir����ƽ���ڱ���ʱ������z����ӽ���0�������᷵�ظ����p������
	// �ӱ����������ǣ�����ڴ�ֱ����ƽ�棬�����нǶȿ���ƽ��ʱ�������̶ȵ�����p�������Ӷ��������������ƫ�ƣ����ӽ��ϻ��ø������ʵ��
	vec2 p = viewDir.xy / viewDir.z * (height * heightScale);  
//	vec2 p = viewDir.xy * (height * heightScale);
	return texCoords - p;
}

// �����Ӳ�ӳ��
vec2 parallaxMapping2(vec2 texCoords, vec3 viewDir){

	//	const float numLayers = 10;  // �㼶����

	// С���ɣ����ݹ۲췽��ĽǶȵ����㼶����������ֱ��һ�������ʱ��λ��һ����һ���Ƕȿ�ʱС
	const float minLayers = 8;
	const float maxLayers = 32;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

	float layerDepth = 1.0 / numLayers;  // ÿһ������
	float currentLayerDepth = 0.0;  // ��ǰ���

	vec2 p = viewDir.xy * heightScale;
	vec2 deltaTexCoords = p / numLayers;  // ÿһ������p�����ƶ��Ĳ�ֵ

	vec2 currentTexCoords = texCoords;
	float curretnDepthMapValue = texture(dispMap, texCoords).r;
	while(currentLayerDepth < curretnDepthMapValue){
		currentTexCoords -= deltaTexCoords;
		curretnDepthMapValue = texture(dispMap, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}
	return currentTexCoords;
}

void main(){
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	vec2 texCoords = fs_in.TexCoords;

//	texCoords = parallaxMapping(texCoords, viewDir);  // �����Ӳ�ӳ��
	texCoords = parallaxMapping2(texCoords, viewDir);  // ���ж����Ӳ�ӳ��


	// ���������곬��[0, 1]��������Ļ��Ʒ�ʽ��Ӱ�죬�����ᵼ����ʵ��Ч�����򵥴�����ǵ��������곬����Χʱ���Ͷ������Ƭ��
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0){
		discard;
	}

	vec3 color = texture(diffuseMap, texCoords).rgb;

	vec3 normal = texture(normalMap, texCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

//	vec3 normal = normalize(fs_in.TangentNormal);

	vec3 lightColor = vec3(0.2);

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