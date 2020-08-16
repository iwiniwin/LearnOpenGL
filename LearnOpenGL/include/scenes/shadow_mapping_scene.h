#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float planeVertices2[];
extern int planeVertices2Size;
extern float fullScreenQuadVertices[];
extern int fullScreenQuadVerticesSize;

class ShadowMappingScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	Shader* shader;
	unsigned int VAO;
	unsigned int texture;
	Shader* cubeShader;
	unsigned int cubeVAO;
	Shader* depthShader;
	Shader* quadShder;

	unsigned int quadVAO;

	unsigned int depthMapFBO;
	unsigned int depthMap;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;  // �����ͼ�ķֱ���

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		depthShader = new Shader("shaders\\simple_depth_shader.vs", "shaders\\empth.fs");
		shader = new Shader("shaders\\shader.vs", "shaders\\shadow_mapping.fs");
		cubeShader = new Shader("shaders\\shader.vs", "shaders\\single_tex_shader.fs");
		quadShder = new Shader("shaders\\ndc_shader.vs", "shaders\\single_tex_shader.fs");
		initVAO();

		initFBO();

		glEnable(GL_DEPTH_TEST);
	}

	void initVAO() {
		glGenVertexArrays(1, &VAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, planeVertices2Size, planeVertices2, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);

		texture = loadTextureFromFile("textures\\wood.png");
		shader->use();
		shader->setInt("floorTexture", 0);

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

		glGenVertexArrays(1, &quadVAO);
		unsigned int screenVBO;
		glGenBuffers(1, &screenVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, fullScreenQuadVerticesSize, &fullScreenQuadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);

		this->quadShder->use();
		this->quadShder->setInt("texture1", 0);
	}

	void initFBO() {
		
		glGenFramebuffers(1, &depthMapFBO);

		// ����һ��2D�����ṩ��֡�������Ȼ���ʹ��
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		// ��������Ⱦ�����ͼ��������ɫ����û���ã�����֡�������Ҫ�и���ɫ����
		// ͨ�������ͻ��ƻ�������ΪNONE��������OpenGL���ǲ�ʹ���κ���ɫ������������Ⱦ
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		// ��Ϊ��һ�����й��߶�ƽ�еĶ���⣬����ʹ������ͶӰ����
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		depthShader->use();
		depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// 1. ��Ⱦ�����ͼ
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);  // �ǵõ��ã���Ϊ��Ӱ��ͼ�����ʹ��ڵķֱ��ʲ�һ��
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		depthShader->setMat4("model", model);

		// ���Ƶذ�
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// ����������
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ����
		// ���������ͼ���ı���
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(quadVAO);
		quadShder->use();
		//quadShder
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glBindVertexArray(0);


		// 2. ������һ����Ⱦ������ͬʱʹ����һ�����ɵ������ͼ

		

		// ���Ƶذ�
		//shader->use();
		//shader->setMat4("projection", projection);
		//shader->setMat4("view", view);
		//shader->setMat4("model", model);
		//glBindVertexArray(VAO);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//// ����������
		//cubeShader->use();
		//cubeShader->setMat4("model", model);
		//cubeShader->setMat4("view", view);
		//cubeShader->setMat4("projection", projection);
		//cubeShader->setVec3("cameraPos", camera.Position);
		//glBindVertexArray(cubeVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void renderScene() {
		
		
	}


	virtual void destroy() {
		delete(shader);
		delete(depthShader);
		delete(cubeShader);
		delete(quadShder);
	}
};