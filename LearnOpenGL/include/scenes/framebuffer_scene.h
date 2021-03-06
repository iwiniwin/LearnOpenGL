#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float vertices[];
extern int verticesSize;
extern float planeVertices[];
extern int planeVerticesSize;
extern float fullScreenQuadVertices[];
extern int fullScreenQuadVerticesSize;

class FrameBufferScene : public Scene {

public:
	Camera camera;
	unsigned int cubeVAO;
	unsigned int planeVAO;
	unsigned int screenVAO;
	unsigned int cubeTexture;
	unsigned int planeTexture;
	unsigned int floorTexture;
	unsigned int fbo;
	unsigned int texture;
	Shader* shader;
	Shader* screenShader;
	float width;
	float height;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		
		initVAO();
		
		initFramebuffer();
		
		// 设置线框模式
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void initFramebuffer() {
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// 纹理附件
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 避免利用卷积核进行采样时，采样到纹理的另一边，导致屏幕边缘出现奇怪的条纹
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		/*
		将纹理附件附加到帧缓冲上
		参数1，帧缓冲类型的目标
		参数2，附加的附件的类型
		参数3，附加的纹理的类型
		参数4，附加的实际纹理
		参数5，Mipmap level
		*/
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		
		// 创建用于深度和模板缓冲的渲染缓冲对象附件
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		// 创建渲染缓冲对象
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		
		// 把渲染缓冲对象附加到帧缓冲上，注意下面的枚举值不要写错了
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cerr << "check framebuffer error" << endl;
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glDeleteFramebuffers(1, &fbo);
	}

	void initVAO() {
		this->shader = new Shader("shaders\\shader.vs", "shaders\\depth_shader.fs");
		this->screenShader = new Shader("shaders\\ndc_shader.vs", "shaders\\convolution_kernel_shader.fs");

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

		cubeTexture = loadTextureFromFile("textures\\container2.png");
		floorTexture = loadTextureFromFile("textures\\container.jpg");

		this->shader->use();
		this->shader->setInt("texture1", 0);

		glGenVertexArrays(1, &screenVAO);
		unsigned int screenVBO;
		glGenBuffers(1, &screenVBO);
		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, fullScreenQuadVerticesSize, &fullScreenQuadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);

		this->screenShader->use();
		this->screenShader->setInt("texture1", 0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);

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

		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		this->shader->setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		this->screenShader->use();
		glBindVertexArray(this->screenVAO);
		// 绘制的是一个简单的四边形并不需要深度测试
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	virtual void destroy() {
		delete(this->shader);
		delete(this->screenShader);
	}
};