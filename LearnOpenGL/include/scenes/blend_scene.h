#pragma once
#include <memory>
#include "scene.h"
#include "model.h"
#include <map>

extern float vertices[];
extern int verticesSize;
extern float planeVertices[];
extern int planeVerticesSize;
extern float quadrangleVertices[];
extern int quadrangleVerticesSize;

class BlendScene : public Scene {

public:
	Camera camera;
	unsigned int cubeVAO;
	unsigned int planeVAO;
	unsigned int vegetationVAO;

	unsigned int cubeTexture;
	unsigned int floorTexture;
	unsigned int grassTexture;
	unsigned int windowTexture;
	
	Shader* shader;
	float width;
	float height;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_ALWAYS);
		glDepthFunc(GL_LESS);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		this->shader = new Shader("shaders\\shader.vs", "shaders\\blend_shader.fs");

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

		glGenVertexArrays(1, &planeVAO);
		unsigned int planeVBO;
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, planeVerticesSize, &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		glGenVertexArrays(1, &vegetationVAO);
		unsigned int vegetationVBO;
		glGenBuffers(1, &vegetationVBO);
		glBindVertexArray(vegetationVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vegetationVBO);
		glBufferData(GL_ARRAY_BUFFER, quadrangleVerticesSize, &quadrangleVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);


		cubeTexture = loadTextureFromFile("textures\\container2.png");
		floorTexture = loadTextureFromFile("textures\\container.jpg");
		grassTexture = loadTextureFromFile("textures\\grass.png");
		windowTexture = loadTextureFromFile("textures\\blending_transparent_window.png");

		this->shader->use();
		this->shader->setInt("texture1", 0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->shader->use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), this->width / this->height, 0.1f, 100.0f);
		this->shader->setMat4("view", view);
		this->shader->setMat4("projection", projection);

		// cubes
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		this->shader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		this->shader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// plane
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		this->shader->setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// vegetation
		vector<glm::vec3> vegetation;
		vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
		vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
		vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
		vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
		vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

		// 对透明物体进行排序
		map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < vegetation.size(); i++) {
			float distance = glm::length(camera.Position - vegetation[i]);
			sorted[distance] = vegetation[i];
		}

		glBindVertexArray(vegetationVAO);
		//glBindTexture(GL_TEXTURE_2D, grassTexture);
		glBindTexture(GL_TEXTURE_2D, windowTexture);

		for (map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			this->shader->setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/*for (unsigned int i = 0; i < vegetation.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, vegetation[i]);
			this->shader->setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}*/

		glBindVertexArray(0);

	}
	virtual void destroy() {
		delete(this->shader);
	}
};