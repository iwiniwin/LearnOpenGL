#pragma once
#include <memory>
#include "scene.h"
#include "model.h"
#include <random>

class SSAOScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	unsigned int quadVAO;
	unsigned int cubeVAO;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedo;

	unsigned int ssaoFBO;  // ���ڴ洢SSAO�׶εĽ��
	unsigned int ssaoColorBuffer;

	unsigned int noiseTexture;

	Shader* geometryPassShader;
	Shader* lightPassShder;
	Shader* ssaoShader;
	Shader* ssaoBlurShader;

	Model* nanosuitModel;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		nanosuitModel = new Model("nanosuit\\nanosuit.obj");
		geometryPassShader = new Shader("shaders\\ssao_scene.vs", "shaders\\ssao_mrt.frag");;
		lightPassShder = new Shader("shaders\\bloom_gaussian_blur.vs", "shaders\\ssao_scene.frag");
		ssaoShader = new Shader("shaders\\bloom_gaussian_blur.vs", "shaders\\ssao.frag");;
		ssaoBlurShader = new Shader("shaders\\bloom_gaussian_blur.vs", "shaders\\ssao_blur.frag");

		lightPassShder->use();
		lightPassShder->setInt("gPosition", 0);
		lightPassShder->setInt("gNormal", 1);
		lightPassShder->setInt("gAlbedo", 2);
		lightPassShder->setInt("ssao", 3);

		ssaoShader->use();
		ssaoShader->setInt("gPosition", 0);
		ssaoShader->setInt("gNormal", 1);
		ssaoShader->setInt("texNoise", 2);

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
		glBufferData(GL_ARRAY_BUFFER, tunnelVerticesSize, tunnelVertices, GL_STATIC_DRAW);
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// ����64������������ÿ�������������ᱻ����ƫ����������
		uniform_real_distribution<float> randomFloats(0.0, 1.0);  // �������������Χ0.0 - 1.0
		default_random_engine generator;
		vector<glm::vec3> ssaoKernel;
		for (unsigned int i = 0; i < 64; i++) {
			glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,  // ��Χ�� -1 �� 1
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)  // ��Χ�� 0 �� 1 ��Ϊ�Ƿ��������
			);
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / 64.0;
			scale = lerp(0.1f, 1.0f, scale * scale);  // ���ٲ�ֵ
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		// ����һЩ����Ե�����������
		// �����ת��������
		vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++) {
			// ���������Ӧ������+z���������߷�������ת�������趨z������0
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f
			);
			ssaoNoise.push_back(noise);
		}
		// ����һ�����������ת������4x4����
		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void initFBO() {
		// ����G-Buffer
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// λ��-�����ɫ����
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		// ʹ��GL_RGB���洢λ����Ϣ����Ϊֻ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// ������ɫ����
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// ��ɫ + ����ǿ�Ȼ���
		glGenTextures(1, &gAlbedo);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

		// ����OpenGLҪ��Ⱦ�������ɫ����
		unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
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

		// ���ڴ洢SSAO�׶εĽ��
		glGenFramebuffers(1, &ssaoFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glGenTextures(1, &ssaoColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		// ����SSAO�Ľ����һ���Ҷ�ֵ������ֻ��Ҫ����ĺ�ɫ����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Framebuffer not complete !" << endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float lerp(float a, float b, float f) {
		return a + f * (b - a);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
		glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 50.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		geometryPassShader->use();
		geometryPassShader->setMat4("projection", projection);
		geometryPassShader->setMat4("view", view);

		// room
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
		geometryPassShader->setMat4("model", model);
		geometryPassShader->setInt("inverse_normals", 1);
		renderCube();
		geometryPassShader->setInt("inverse_normals", 0);
		// backpack model on the floor
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 8.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(1.0f));
		geometryPassShader->setMat4("model", model);
		nanosuitModel->draw(*geometryPassShader);
	
		// 1. ���δ���׶Σ���Ⱦ��G-Buffer

		// 2. ʹ��G-Buffer�ļ���������ȾSSAO����
		//glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		//glClear(GL_COLOR_BUFFER_BIT);
		//ssaoShader->use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, gPosition);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, gNormal);
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, noiseTexture);
		//renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. ���մ���׶Σ���Ⱦ���ճ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightPassShder->use();
		// ����Դλ�ô�����ռ�ת�����۲�ռ�
		glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
		lightPassShder->setVec3("light.Position", lightPosView);
		lightPassShder->setVec3("light.Color", lightColor);

		const float linear = 0.09;
		const float quadratic = 0.032;
		lightPassShder->setFloat("light.Linear", linear);
		lightPassShder->setFloat("light.Quadratic", quadratic);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		renderQuad();
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