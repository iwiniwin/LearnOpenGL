#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float vertices[];
extern int verticesSize;

class AntiAliasingScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	Shader* cubeShader;
	unsigned int cubeVAO;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		glEnable(GL_MULTISAMPLE);  // 启用多重采样，想要多重采样生效还需要在窗口创建前调用glfwWindowHint(GLFW_SAMPLES, 4);
		cubeShader = new Shader("shaders\\shader.vs", "shaders\\single_color_shader.fs");
		initVAO();
	}

	void initVAO() {
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

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glBindVertexArray(cubeVAO);
		cubeShader->use();
		cubeShader->setMat4("model", model);
		cubeShader->setMat4("view", view);
		cubeShader->setMat4("projection", projection);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	virtual void destroy() {

	}
};