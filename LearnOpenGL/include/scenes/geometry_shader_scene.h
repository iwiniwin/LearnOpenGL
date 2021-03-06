#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

class GeometryShaderScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	unsigned int VAO;
	Shader* shader;
	Model* nanosuitModel;
	Shader* nanosuitShader;
	Shader* nanosuitExplodeShader;
	Shader* normalDisplayShader;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		shader = new Shader("shaders\\point_shader.vs", "shaders\\single_color_shader.fs", "shaders\\shader.gs");
		this->nanosuitModel = new Model("nanosuit\\nanosuit.obj");
		this->nanosuitShader = new Shader("shaders\\shader.vs", "shaders\\model_diffuse_shader.fs");
		this->nanosuitExplodeShader = new Shader("shaders\\shader.vs", "shaders\\model_diffuse_shader.fs", "shaders\\explode_shader.gs");
		this->normalDisplayShader = new Shader("shaders\\shader.vs", "shaders\\single_color_shader.fs", "shaders\\normal_display_shader.gs");
		initVAO();
	}

	void initVAO() {
		glEnable(GL_DEPTH_TEST);
		glGenVertexArrays(1, &VAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		float data[] = {
			-0.5, 0.5, 1.0f, 0.0f, 0.0f,
			0.5, 0.5, 0.0f, 1.0f, 0.0f,
			-0.5, -0.5, 0.0f, 0.0f, 1.0f,
			0.5, -0.5, 1.0f, 1.0f, 0.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), &data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 绘制点
		//glBindVertexArray(VAO);
		//shader->use();
		//shader->setMat4("model", glm::mat4(1.0f));
		//shader->setMat4("view", camera.GetViewMatrix());
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		//shader->setMat4("projection", projection);
		//glDrawArrays(GL_POINTS, 0, 4);

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 0.15f));

		/*nanosuitExplodeShader->use();
		this->nanosuitExplodeShader->setMat4("projection", projectionMatrix);
		this->nanosuitExplodeShader->setMat4("view", viewMatrix);
		this->nanosuitExplodeShader->setMat4("model", modelMatrix);
		nanosuitExplodeShader->setFloat("time", glfwGetTime());
		nanosuitModel->draw(*nanosuitExplodeShader);*/

		nanosuitShader->use();
		this->nanosuitShader->setMat4("projection", projectionMatrix);
		this->nanosuitShader->setMat4("view", viewMatrix);
		this->nanosuitShader->setMat4("model", modelMatrix);
		nanosuitModel->draw(*nanosuitShader);

		normalDisplayShader->use();
		normalDisplayShader->setMat4("projection", projectionMatrix);
		this->normalDisplayShader->setMat4("view", viewMatrix);
		this->normalDisplayShader->setMat4("model", modelMatrix);
		nanosuitModel->draw(*normalDisplayShader);
	}
	virtual void destroy() {
		delete(shader);
		delete(nanosuitModel);
		delete(nanosuitExplodeShader);
		delete(normalDisplayShader);
		delete(nanosuitShader);
	}
};