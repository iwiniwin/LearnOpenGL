#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main(){
	vec3 color = texture(texture1, TexCoords).rgb;
	vec3 ambient = 0.05 * color;
	
	// diffuse
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(lightPos - FragPos);

	vec3 diffuse = color * max(dot(lightDir, normal), 0.0);

	vec3 specular = vec3(0.3f);
	if(blinn){
		vec3 halfDir = normalize(lightDir + viewDir);
		specular *= pow(max(dot(halfDir, normal), 0.0), 16.0);
		// specular *= pow(max(dot(halfDir, normal), 0.0), 1.0);
	}else{
		vec3 reflectDir = reflect(-lightDir, normal);
		specular *= pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
		// 当反光度很小，比如为1时，phong模型会出现高光区域断层现象
		// specular *= pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
	}

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}