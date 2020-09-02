#version 330 core

out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

// 用以使噪声纹理平铺在屏幕上
// 屏幕的平铺噪声纹理会根据屏幕分辨率除以噪声大小的值来决定
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0);  // 屏幕800*400

// 用来调整效果
const int kernelSize = 64;
const float radius = 1.0;

void main(){

	vec3 fragPos = texture(gPositionDepth, TexCoords).rgb;
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; i ++){
		// 获取样本位置
		vec3 sample = TBN * samples[i];  // 切线 - 观察空间
		sample = fragPos + sample * radius;

		vec4 offset = vec4(sample, 1.0f);
		offset = projection * offset;  // 观察空间 - 裁剪空间
		offset.xyz /= offset.w;  // 透视除法
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = -texture(gPositionDepth, offset.xy).w;
		
		// 范围测试，保证只当被测深度值在取样半径内影响遮蔽因子
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = occlusion;
}