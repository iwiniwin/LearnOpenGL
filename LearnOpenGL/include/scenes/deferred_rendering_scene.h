#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

class DeferredRenderingScene : public Scene {

public:
	Camera camera;
	float width;
	float height;

	Shader* geometryPassShader;
	Shader* lightPassShder;
	Shader* lightShder;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec;

	unsigned int quadVAO;
	unsigned int cubeVAO;

	Model* nanosuitModel;
	std::vector<glm::vec3> objectPositions;

	const unsigned int NR_LIGHTS = 32;
	vector<glm::vec3> lightPositions;
	vector<glm::vec3> lightColor;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f));

		nanosuitModel = new Model("nanosuit\\nanosuit.obj");
		
		objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
		objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
		objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
		objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
		objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
		objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
		objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
		objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
		objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

		// ��Դλ������ɫ
		srand(13);
		for (unsigned int i = 0; i < NR_LIGHTS; i++) {
			float xPos = (rand() % 100 / 100.0) * 6.0 - 3.0;
			float yPos = (rand() % 100 / 100.0) * 6.0 - 4.0;
			float zPos = (rand() % 100 / 100.0) * 6.0 - 3.0;
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

			float rColor = ((rand() % 100) / 200.0f) + 0.5;
			float gColor = ((rand() % 100) / 200.0f) + 0.5;
			float bColor = ((rand() % 100) / 200.0f) + 0.5;
			lightColor.push_back(glm::vec3(rColor, gColor, bColor));
		}

		geometryPassShader = new Shader("shaders\\deferred_rendering_scene.vs", "shaders\\deferred_rendering_mrt.frag");;
		lightPassShder = new Shader("shaders\\bloom_gaussian_blur.vs", "shaders\\deferred_rendering_scene.frag");
		lightShder = new Shader("shaders\\deferred_rendering_scene.vs", "shaders\\bloom_light_box.frag");

		lightPassShder->use();
		lightPassShder->setInt("gPosition", 0);
		lightPassShder->setInt("gNormal", 1);
		lightPassShder->setInt("gAlbedoSpec", 2);

		initVAO();
		initFBO();

		glEnable(GL_DEPTH_TEST);
	}

	void initVAO() {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quadVAO);
		unsigned int quadVBO;
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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
	}

	void initFBO() {
		// ����G-Buffer
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// λ����ɫ����
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		// ʹ��GL_RGB���洢λ����Ϣ����Ϊֻ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// ������ɫ����
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// ��ɫ + ����ǿ�Ȼ���
		glGenTextures(1, &gAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

		// ����OpenGLҪ��Ⱦ�������ɫ����
		unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// �����Ⱦ�������
		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Framebuffer not complete !" << endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		// 1. Geometry Pass

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;
		geometryPassShader->use();
		geometryPassShader->setMat4("projection", projection);
		geometryPassShader->setMat4("view", view);
		for (unsigned int i = 0; i < objectPositions.size(); i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			geometryPassShader->setMat4("model", model);
			nanosuitModel->draw(*geometryPassShader);
		}

		// 2. Lighting Pass , ����G�����е����ݽ��й��ռ���

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightPassShder->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			lightPassShder->setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
			lightPassShder->setVec3("lights[" + to_string(i) + "].Color", lightColor[i]);

			const float constant = 1.0;
			const float linear = 0.7;
			const float quadratic = 1.8;
			lightPassShder->setFloat("lights[" + to_string(i) + "].Linear", linear);
			lightPassShder->setFloat("lights[" + to_string(i) + "].Quadratic", quadratic);
		}
		lightPassShder->setVec3("viewPos", camera.Position);
		renderQuad();

		// ����ӳ���Ⱦ��������Ⱦ

		// ʹ��g-Buffer�е���Ȼ�������
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // д�뵽Ĭ��֡����
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ��Ⱦ��Դ������
		lightShder->use();
		lightShder->setMat4("projection", projection);
		lightShder->setMat4("view", view);
		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f));
			lightShder->setMat4("model", model);
			lightShder->setVec4("color", glm::vec4(lightColor[i], 1.0f));
			renderCube();
		}
	}

	void renderQuad() {
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void renderCube() {
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	virtual void destroy() {

	}
};