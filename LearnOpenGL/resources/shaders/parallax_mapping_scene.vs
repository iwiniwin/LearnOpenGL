#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec3 TangentNormal;
} vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	
	// 创建TBN矩阵
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(vec3(normalMatrix * aTangent));
	vec3 B = normalize(vec3(normalMatrix * aBitangent));  // 副切线其实可以由T和N叉积得到
	vec3 N = normalize(vec3(normalMatrix * aNormal));
	mat3 TBN = mat3(T, B, N);
	// 此时的TBN 用于将向量从切线空间转换到世界空间

	// 在切线空间计算光照，将光源位置，观察位置，顶点位置等变换到切线空间再发送给片段着色器
	TBN = transpose(TBN);  // 因为这里TBN是正交矩阵，逆矩阵等于其转置矩阵
	// 此时的TBN 用于将向量从世界空间转换到切线空间

	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.TexCoords = aTexCoords;

	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	vs_out.TangentNormal = TBN * normalMatrix * aNormal;
}