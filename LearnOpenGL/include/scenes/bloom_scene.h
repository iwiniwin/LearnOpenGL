#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float cubeVertices[];
extern int cubeVerticesSize;

class BloomScene : public Scene {

public:
	Camera camera;
	float width;
	float height;

	// 正常渲染场景，以及提取较亮区域使用的shader
	Shader* shader;  
	Shader* lightShder; 
	// 高斯模糊使用的shader
	Shader* blurShader;  
	// 混合模糊纹理与原来纹理使用的shader
	Shader* blendShader;

	unsigned int woodTexture;
	unsigned int containerTexture;

	unsigned int cubeVAO;
	unsigned int quadVAO;
	unsigned int hdrFBO;

	unsigned int colorBuffers[2];  // 两个颜色附件，一个用来存储正常的图像，一个用来存储提取的较亮的图像

	unsigned int pingpongFBO[2];  // 乒乓FBO，用来实现两步高斯模糊
	unsigned int pingpongBuffer[2];

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		woodTexture = loadTextureFromFile("textures\\wood.png");
		containerTexture = loadTextureFromFile("textures\\container2.png");

		shader = new Shader("shaders\\bloom_scene.vs", "shaders\\bloom_scene.frag");
		lightShder = new Shader("shaders\\bloom_scene.vs", "shaders\\bloom_light_box.frag");
		blurShader = new Shader("shaders\\bloom_gaussian_blur.vs", "shaders\\bloom_gaussian_blur.frag");
		blendShader = new Shader("shaders\\bloom_gaussian_blur.vs", "shaders\\bloom_blend.frag");
		shader->use();
		shader->setInt("diffuseMap", 0);

		blurShader->use();
		blurShader->setInt("image", 0);

		blendShader->use();
		blendShader->setInt("scene", 0);
		blendShader->setInt("blur", 1);

		initVAO();
		initFBO();

		glEnable(GL_DEPTH_TEST);
	}

	void initVAO() {
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
		glGenFramebuffers(1, &hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

		glGenTextures(2, colorBuffers);
		// 两个颜色附件，一个用来存储正常的图像，一个用来存储提取的较亮的图像
		for (unsigned int i = 0; i < 2; i++) {
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
		}

		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		// 显式告知OpenGL渲染到多个颜色缓冲，否则OpenGL只会渲染到帧缓冲的第一个颜色附件，而忽略其他的
		glDrawBuffers(2, attachments); 

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Framebuffer not complete !" << endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 乒乓FBO，用来实现两步高斯模糊
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongBuffer);
		for (unsigned int i = 0; i < 2; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				cout << "Framebuffer not complete !" << endl;
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		// 1. 正常渲染场景，同时利用MRT，提取较亮区域
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// lighting info
		// positions
		vector<glm::vec3> lightPositions;
		lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
		lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
		lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
		lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
		// colors
		vector<glm::vec3> lightColors;
		lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
		lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
		lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
		lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		shader->use();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			shader->setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
			shader->setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
		}
		shader->setVec3("viewPos", camera.Position);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
		shader->setMat4("model", model);
		renderCube();

		glBindTexture(GL_TEXTURE_2D, containerTexture);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
		model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(1.25));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
		model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		shader->setMat4("model", model);
		renderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMat4("model", model);
		renderCube();

		// 绘制灯光
		lightShder->use();
		lightShder->setMat4("projection", projection);
		lightShder->setMat4("view", view);
		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(lightPositions[i]));
			model = glm::scale(model, glm::vec3(0.25f));
			lightShder->setMat4("model", model);
			lightShder->setVec4("color", glm::vec4(lightColors[i], 1.0f));
			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. 两步高斯模糊，水平垂直各5次
		bool horizontal = true, firstIteration = true;
		unsigned int amount = 10;
		blurShader->use();
		for (unsigned int i = 0; i < amount; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			blurShader->setBool("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);
			renderQuad();
			horizontal = !horizontal;
			if (firstIteration)
				firstIteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// 3, 把模糊后的纹理与原来的纹理进行混合
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		blendShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
		blendShader->setInt("bloom", 1);  // 是否开启bloom
		blendShader->setFloat("exposure", 1.0f);
		renderQuad();
	}

	void renderCube() {
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void renderQuad() {
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	virtual void destroy() {

	}
};