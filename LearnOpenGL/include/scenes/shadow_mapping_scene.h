#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float planeVertices2[];
extern int planeVertices2Size;

class ShadowMappingScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	Shader* shader;
	unsigned int VAO;
	unsigned int texture;
	Shader* cubeShader;
	unsigned int cubeVAO;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		shader = new Shader("shaders\\shader.vs", "shaders\\shadow_mapping.fs");
		cubeShader = new Shader("shaders\\shader.vs", "shaders\\single_tex_shader.fs");
		initVAO();

		glEnable(GL_DEPTH_TEST);
	}

	void initVAO() {
		glGenVertexArrays(1, &VAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, planeVertices2Size, planeVertices2, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);

		texture = loadTextureFromFile("textures\\wood.png");
		shader->use();
		shader->setInt("floorTexture", 0);

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

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shader->setMat4("model", model);

		glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
		shader->use();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE0, texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 绘制立方体
		cubeShader->use();
		cubeShader->setMat4("model", model);
		cubeShader->setMat4("view", view);
		cubeShader->setMat4("projection", projection);
		cubeShader->setVec3("cameraPos", camera.Position);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	virtual void destroy() {

	}
};