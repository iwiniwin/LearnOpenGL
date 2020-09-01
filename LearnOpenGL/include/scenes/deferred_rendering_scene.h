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

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec;

	unsigned int quadVAO;

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

		// 光源位置与颜色
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
	}

	void initFBO() {
		// 构建G-Buffer
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// 位置颜色缓冲
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		// 使用GL_RGB来存储位置信息，因为只有三个分量
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// 法线颜色缓冲
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// 颜色 + 镜面强度缓冲
		glGenTextures(1, &gAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

		// 告诉OpenGL要渲染到多个颜色缓冲
		unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// 深度渲染缓冲对象
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
		// 清除颜色缓冲和深度缓冲
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

		// 2. Lighting Pass , 利用G缓冲中的数据进行光照计算

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
	}

	void renderQuad() {
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}


	virtual void destroy() {

	}
};