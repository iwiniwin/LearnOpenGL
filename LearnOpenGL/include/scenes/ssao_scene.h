#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

class SSAOScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	unsigned int quadVAO;

	unsigned int gBuffer;
	unsigned int gPositionDepth, gNormal, gAlbedoSpec;

	unsigned int ssaoFBO;  // ���ڴ洢SSAO�׶εĽ��
	unsigned int ssaoColorBuffer;

	unsigned int noiseTexture;

	Shader* ssaoShader;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		initVAO();
		initFBO();
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


		// �����ת��������
		vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++) {
			glm::vec3 noise();
			ssaoNoise.push_back(noise);
		}
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
		glGenTextures(1, &gPositionDepth);
		glBindTexture(GL_TEXTURE_2D, gPositionDepth);
		// ʹ��GL_RGB���洢λ����Ϣ����Ϊֻ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		uniform_real_distribution<float> randomFloats(0.0, 1.0);  // �������������Χ0.0 - 1.0
		vector<glm::vec3> ssaoKernel;
		for (unsigned int i = 0; i < 64; i++) {
			//glm::vec3 sample(randomFloats() * 2.0 - 1.0,);
			sample = glm::normalize(sample);
			sample *= randomFloats();
			float scale = float(i) / 64.0;
			scale = lerp(0.1f, 1.0f, scale * scale);  // ���ٲ�ֵ
			sample *= scale;
			ssaoKernel.push_back(sample);
		}


		// 1. ���δ���׶Σ���Ⱦ��G-Buffer

		// 2. ʹ��G-Buffer�ļ���������ȾSSAO����
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		ssaoShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPositionDepth);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. ���մ���׶Σ���Ⱦ���ճ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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