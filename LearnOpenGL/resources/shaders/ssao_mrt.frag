#version 330 core
layout (location = 0) out vec4 glPositionDepth;  // 指定片段着色器写入到哪个颜色缓冲，前提是帧缓冲中绑定了多个颜色缓冲
layout (location = 1) out vec3 gNormal;  // 指定片段着色器写入到哪个颜色缓冲，MRT
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

const float NEAR = 0.1f;  // 近平面
const float FAR = 50.0f;  // 远平面

// 提取出的线性深度是在观察空间中的
float linearizeDepth(float depth){
	float z = depth * 2.0 - 1.0;  // 回到ndc
	return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main(){
	glPositionDepth.rgb = FragPos;
	glPositionDepth.a = linearizeDepth(gl_FragCoord.z);
	gNormal = Normal;
	gAlbedoSpec.rgb = vec3(0.95f);
}