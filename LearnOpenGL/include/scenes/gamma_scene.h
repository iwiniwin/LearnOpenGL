#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float planeVertices2[];
extern int planeVertices2Size;

class GammaScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	Shader* shader;
	unsigned int VAO;
	unsigned int texture;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		shader = new Shader("shaders\\shader.vs", "shaders\\gamma_correction.fs");
		initVAO();
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
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
		shader->use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shader->setMat4("model", model);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE0, texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	virtual void destroy() {

	}
};