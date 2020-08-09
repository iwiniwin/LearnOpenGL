#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float smallQuadVertices[];
extern int smallQuadVerticesSize;

class InstancingScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	unsigned int VAO;
	Shader* shader;
	Shader* planetShader;
	Shader* rockShader;
	Model* planetModel;
	Model* rockModel;

	unsigned int amount = 100000;
	glm::mat4* modelMatrices;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 50.0f, 215.0f));

		shader = new Shader("shaders\\instancing_shader.vs", "shaders\\single_color_shader.fs");

		planetShader = new Shader("shaders\\planet_instancing_shader.vs", "shaders\\model_diffuse_shader.fs");
		rockShader = new Shader("shaders\\rock_instancing_shader.vs", "shaders\\model_diffuse_shader.fs");
		planetModel = new Model("planet\\planet.obj");
		rockModel = new Model("rock\\rock.obj");

		initMatrics();
		initVAO();
	}

	void initVAO() {
		glGenVertexArrays(1, &VAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, smallQuadVerticesSize, smallQuadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));

		glm::vec2 translations[100];
		int index = 0;
		float offset = 0.1f;
		for (int y = -10; y < 10; y += 2) {
			for (int x = -10; x < 10; x += 2) {
				glm::vec2 translation;
				translation.x = (float)x / 10.0f + offset;
				translation.y = (float)y / 10.0f + offset;
				translations[index++] = translation;
			}
		}
		unsigned int instanceVBO;
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
		/*
			����OpenGL��ʲôʱ����¶������Ե����ݵ���һ������
			����1����Ҫ���õĶ������Ե�λ��ֵ
			����2�������Գ�����Ĭ����������Գ�����0������OpenGL��Ҫ�ڶ�����ɫ����ÿ�ε������¶������ԣ�
				����Ϊ1���Ǹ���OpenGL��Ҫ����Ⱦһ����ʵ��ʱ���¶������ԣ�����Ϊ2����ʾÿ����ʵ������һ�����ԡ��Դ�����
		*/
		glVertexAttribDivisor(2, 1);
		glBindVertexArray(0);

		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
		for (unsigned int i = 0; i < rockModel->meshes.size(); i++) {
			unsigned int VAO = rockModel->meshes[i].VAO;
			glBindVertexArray(VAO);
			// ��������
			GLsizei vec4Size = sizeof(glm::vec4);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * 2));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size * 3));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);
			glBindVertexArray(0);
		}
	}

	void initMatrics() {
		modelMatrices = new glm::mat4[amount];
		srand(glfwGetTime());  // ��ʼ���������
		float radius = 150.0f;
		float offset = 25.0f;
		for (unsigned int i = 0; i < amount; i++) {
			glm::mat4 model;
			// λ�ƣ��ֲ��ڰ뾶Ϊradius��Բ���ϣ�ƫ�Ƶķ�Χ��[-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f;  // �����Ǵ��ĸ߶ȱ�x��z�Ŀ��С
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// ���ţ���0.05��0.25֮������
			float scale = (rand() % 20) / 100.0f + 0.05f;
			model = glm::scale(model, glm::vec3(scale));

			// ��ת������һ�����룩���ѡ�����ת�������������ת
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			modelMatrices[i] = model;
		}
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glBindVertexArray(VAO);
		//shader->use();
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
		//glBindVertexArray(0);

		
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 1000.0f);
		// ��������
		planetShader->use();
		planetShader->setMat4("view", view);
		planetShader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		planetShader->setMat4("model", model);
		planetModel->draw(*planetShader);

		//// ����С����

		// ��ͨ��������
		//for (unsigned int i = 0; i < amount; i++) {
		//	planetShader->setMat4("model", modelMatrices[i]);
		//	rockModel->draw(*planetShader);
		//}

		// Instancing
		rockShader->use();
		rockShader->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rockModel->textures_loaded[0].id);
		rockShader->setMat4("view", view);
		rockShader->setMat4("projection", projection);
		for (unsigned int i = 0; i < rockModel->meshes.size(); i++) {
			glBindVertexArray(rockModel->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, rockModel->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}
	}
	virtual void destroy() {
		delete(shader);
		delete(planetShader);
		delete(rockShader);
		delete(planetModel);
		delete(rockModel);
	}
};