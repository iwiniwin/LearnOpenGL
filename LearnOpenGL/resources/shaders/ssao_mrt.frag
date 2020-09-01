#version 330 core
layout (location = 0) out vec4 glPositionDepth;  // ָ��Ƭ����ɫ��д�뵽�ĸ���ɫ���壬ǰ����֡�����а��˶����ɫ����
layout (location = 1) out vec3 gNormal;  // ָ��Ƭ����ɫ��д�뵽�ĸ���ɫ���壬MRT
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

const float NEAR = 0.1f;  // ��ƽ��
const float FAR = 50.0f;  // Զƽ��

// ��ȡ��������������ڹ۲�ռ��е�
float linearizeDepth(float depth){
	float z = depth * 2.0 - 1.0;  // �ص�ndc
	return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main(){
	glPositionDepth.rgb = FragPos;
	glPositionDepth.a = linearizeDepth(gl_FragCoord.z);
	gNormal = Normal;
	gAlbedoSpec.rgb = vec3(0.95f);
}