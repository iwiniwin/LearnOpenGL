#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float planeVertices2[];
extern int planeVertices2Size;
extern float planeVertices3[];
extern int planeVertices3Size;
extern float cubeVertices[];
extern int cubeVerticesSize;

class NormalMappingScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	unsigned int VAO;
	unsigned int cubeVAO;
	unsigned int diffuseMap;
	unsigned int normalMap;
	Shader* shader;
	Shader* cubeShader;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		shader = new Shader("shaders\\normal_mapping_scene.vs", "shaders\\normal_mapping_scene.frag");
		cubeShader = new Shader("shaders\\normal_mapping_scene.vs", "shaders\\single_color_shader.fs");
		diffuseMap = loadTextureFromFile("textures\\brickwall.jpg");
		normalMap = loadTextureFromFile("textures\\brickwall_normal.jpg");
		shader->use();
		shader->setInt("diffuseMap", 0);
		shader->setInt("normalMap", 1);

		initVAO();
		glEnable(GL_DEPTH_TEST);
	}

	void initVAO() {
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		glGenVertexArrays(1, &VAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, planeVertices3Size, planeVertices3, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);

		glGenVertexArrays(1, &cubeVAO);
		unsigned int cubeVBO;
		glGenBuffers(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, cubeVerticesSize, cubeVertices, GL_STATIC_DRAW);
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

		glm::mat4 projection = glm::perspective(camera.Zoom, width / height, 0.01f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.1f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		shader->use();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
		shader->setMat4("model", model);

		shader->setVec3("lightPos", lightPos);
		shader->setVec3("viewPos", camera.Position);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		cubeShader->use();
		cubeShader->setMat4("projection", projection);
		cubeShader->setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f));
		model = glm::translate(model, lightPos);
		cubeShader->setMat4("model", model);
		renderCube();
	}

	void renderCube() {
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	virtual void destroy() {

	}
};