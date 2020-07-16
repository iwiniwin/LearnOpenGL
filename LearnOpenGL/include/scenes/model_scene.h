#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float vertices[];
extern unsigned int indices[];
extern int verticesSize;
extern int indicesSize;

class ModelScene : public Scene {

public:
	Camera camera;
	Model* nanosuitModel;
	Shader* nanosuitShader;
	Shader* lightShader;
	unsigned int lightVAO;
	float width;
	float height;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		glEnable(GL_DEPTH_TEST);
		this->nanosuitModel = new Model("nanosuit\\nanosuit.obj");
		this->nanosuitShader = new Shader("shaders\\shader.vs", "shaders\\model_shader.fs");
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		this->width = width;
		this->height = height;
		initLight();
	}

	void initLight() {
		this->lightShader = new Shader("shaders\\shader.vs", "shaders\\light_shader.fs");
		glGenVertexArrays(1, &lightVAO);
		unsigned int lightVBO;
		glGenBuffers(1, &lightVBO);
		glBindVertexArray(lightVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
		glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// 解绑VAO
		glBindVertexArray(0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->nanosuitShader->use();
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 viewMatrix = camera.GetViewMatrix();
		this->nanosuitShader->setMat4("projection", projectionMatrix);
		this->nanosuitShader->setMat4("view", viewMatrix);

		// 点光源
		glm::vec3 pointLightPos[] = {
			glm::vec3(0.0f,  2.4f,  0.8f),
			glm::vec3(0.0f, 2.6f, -0.8f),
		};

		this->nanosuitShader->setVec3("viewPos", camera.Position);
		this->nanosuitShader->setFloat("material.shininess", 64.0f);

		this->nanosuitShader->setVec3("pointLights[0].position", pointLightPos[0]);
		this->nanosuitShader->setFloat("pointLights[0].constant", 1.0f);
		this->nanosuitShader->setFloat("pointLights[0].linear", 0.09f);
		this->nanosuitShader->setFloat("pointLights[0].quadratic", 0.032f);
		this->nanosuitShader->setVec3("pointLights[0].ambient", glm::vec3(0.5f));
		this->nanosuitShader->setVec3("pointLights[0].diffuse", glm::vec3(0.8f));
		this->nanosuitShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

		this->nanosuitShader->setVec3("pointLights[1].position", pointLightPos[1]);
		this->nanosuitShader->setFloat("pointLights[1].constant", 1.0f);
		this->nanosuitShader->setFloat("pointLights[1].linear", 0.09f);
		this->nanosuitShader->setFloat("pointLights[1].quadratic", 0.032f);
		this->nanosuitShader->setVec3("pointLights[1].ambient", glm::vec3(0.5f));
		this->nanosuitShader->setVec3("pointLights[1].diffuse", glm::vec3(0.8f));
		this->nanosuitShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
		this->nanosuitShader->setMat4("model", modelMatrix);
		this->nanosuitModel->draw(*this->nanosuitShader);

		// 绘制光源
		this->lightShader->use();
		this->lightShader->setMat4("view", viewMatrix);
		this->lightShader->setMat4("projection", projectionMatrix);
		glBindVertexArray(lightVAO);
		for (int i = 0; i < 2; i++) {
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, pointLightPos[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.04f));
			this->lightShader->setMat4("model", lightModel);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
	}
	virtual void destroy() {
		delete(this->nanosuitModel);
		delete(this->nanosuitShader);
		delete(this->lightShader);
	}
};