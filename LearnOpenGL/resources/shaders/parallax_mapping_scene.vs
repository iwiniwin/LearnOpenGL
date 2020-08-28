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
	
	// ����TBN����
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(vec3(normalMatrix * aTangent));
	vec3 B = normalize(vec3(normalMatrix * aBitangent));  // ��������ʵ������T��N����õ�
	vec3 N = normalize(vec3(normalMatrix * aNormal));
	mat3 TBN = mat3(T, B, N);
	// ��ʱ��TBN ���ڽ����������߿ռ�ת��������ռ�

	// �����߿ռ������գ�����Դλ�ã��۲�λ�ã�����λ�õȱ任�����߿ռ��ٷ��͸�Ƭ����ɫ��
	TBN = transpose(TBN);  // ��Ϊ����TBN��������������������ת�þ���
	// ��ʱ��TBN ���ڽ�����������ռ�ת�������߿ռ�

	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.TexCoords = aTexCoords;

	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	vs_out.TangentNormal = TBN * normalMatrix * aNormal;
}