#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float skyboxVertices[];
extern int skyboxVerticesSize;
extern float vertices[];
extern int verticesSize;

class CubemapScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	unsigned int VAO;
	unsigned int cubeVAO;
	unsigned int cubemapTexture;
	unsigned int cubeTexture;
	Shader* skyboxShader;
	Shader* cubeShader;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		
		skyboxShader = new Shader("shaders\\cubemap_shader.vs", "shaders\\single_cubemap_shader.fs");
		cubeShader = new Shader("shaders\\shader.vs", "shaders\\depth_shader.fs");

		initVAO();
		vector<string> faces{
			"skybox\\right.jpg",
			"skybox\\left.jpg",
			"skybox\\top.jpg",
			"skybox\\bottom.jpg",
			"skybox\\front.jpg",
			"skybox\\back.jpg",
		};
		cubemapTexture = loadCubemap(faces);
		cubeTexture = loadTextureFromFile("textures\\container2.png");
	}

	void initVAO() {
		glGenVertexArrays(1, &VAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, skyboxVerticesSize, &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		glGenVertexArrays(1, &cubeVAO);
		unsigned int cubeVBO;
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, verticesSize, &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}

	unsigned int loadCubemap(vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++) {
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				cerr << "Texture failed to load at path : " << faces[i] << endl;
			}
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		glm::mat4 model = glm::mat4(1.0f);
		// 移除位移的影响
		//glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);

		cubeShader->use();
		cubeShader->setMat4("model", model);
		cubeShader->setMat4("view", glm::mat4(camera.GetViewMatrix()));
		cubeShader->setMat4("projection", projection);

		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthFunc(GL_LEQUAL);

		skyboxShader->use();
		skyboxShader->setMat4("view", view);
		skyboxShader->setMat4("projection", projection);

		// 设置观察和投影矩阵
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}
	virtual void destroy() {
		delete(skyboxShader);
		delete(cubeShader);
	}
};