#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
uniform sampler2D texture1;
uniform sampler2D texture2;

float near = 0.1;
float far = 100.0;

float linearDepth(float depth){
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(){
	 FragColor = texture(texture2, TexCoords);

	// 深度缓冲可视化
	// FragColor = vec4(vec3(gl_FragCoord.z), 1.0f);

	// 线性深度缓冲可视化
	// float depth = linearDepth(gl_FragCoord.z) / far;
	// FragColor = vec4(vec3(depth), 1.0f);
}