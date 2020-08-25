#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float cubeVertices[];
extern int cubeVerticesSize;

class OmniShadowMappingScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	unsigned int depthMapFBO;
	unsigned int depthCubemap;  // ��������ͼ
	const unsigned SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;  // �����ͼ�ֱ���
	Shader* shader;
	Shader* depthShader;
	unsigned int woodTexture;
	unsigned int cubeVAO;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		
		shader = new Shader("shaders\\omni_shadow_scene.vs", "shaders\\omni_shadow_scene.frag");
		depthShader = new Shader("shaders\\omni_shadow_mapping.vs", "shaders\\omni_shadow_mapping.frag", "shaders\\omni_shadow_mapping.gs");
		shader->use();
		shader->setInt("diffuseTexture", 0);
		shader->setInt("depthMap", 1);

		initVAO();

		initFBO();
	}

	void initVAO() {
		woodTexture = loadTextureFromFile("textures\\wood.png");
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

	void initFBO() {
		glGenFramebuffers(1, &depthMapFBO);

		glGenTextures(1, &depthCubemap);
		// �����������ÿ���棬��������Ϊ2D���ֵ����ͼ��
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		// ֱ�Ӱ���������ͼ���ӳ�֡�������ȸ���
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer not completed" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

		float far = 25.0f;
		// ��ռ��ͶӰ������Ϊ�ǵ��Դ������ʹ��͸��ͶӰ
		// fov����Ϊ90�ȣ�������Ұ�����㹻�󣬱�֤����������������ͼ��ÿһ����
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 1.0f, far);

		// Ϊÿ�������ṩһ����ͬ��view����
		std::vector<glm::mat4> shadowView;
		// ��
		shadowView.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		// ��
		shadowView.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		// ��
		shadowView.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		// ��
		shadowView.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		// ��
		shadowView.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		// Զ
		shadowView.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		// 1. ��Ⱦ�������ͼ
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthShader->use();
		for (int i = 0; i < 6; i++) {
			depthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowView[i]);
		}
		depthShader->setFloat("far_plane", far);
		depthShader->setVec3("lightPos", lightPos);
		renderScene(depthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 2. ������Ⱦ������ʹ�������ͼ
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader->use();
		glm::mat4 projection = glm::perspective(camera.Zoom, width / height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
		shader->setFloat("far_plane", far);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		renderScene(shader);
	}

	void renderScene(Shader* shader) {
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(10.0));
		shader->setMat4("model", model);
		glDisable(GL_CULL_FACE);  // ������������ϣ�������ڲ��ģ����԰�Ĭ���޳��ص�
		shader->setInt("reverse_normals", 1);  // �����ڲ���������ʱ�����Է�ת���ߣ��Ա�֤������Ȼ��Ч
		renderCube();

		shader->setInt("reverse_normals", 0);
		glEnable(GL_CULL_FACE);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0f));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5f));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0f));
		model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		model = glm::scale(model, glm::vec3(1.5f));
		shader->setMat4("model", model);
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