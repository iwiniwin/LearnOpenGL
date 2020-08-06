#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

extern float vertices[];
extern int verticesSize;
extern float ccwVertices[];
extern int ccwVerticesSize;

class TestScene : public Scene {

public:
	Camera camera;
	float width;
	float height;
	Shader* testShader;
	Shader* testShader2;
	Shader* testShader3;
	Shader* testShader4;
	unsigned int VAO;
	unsigned int UBO;
	unsigned int cubeVAO;
	unsigned int tex0;
	unsigned int tex1;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		testShader = new Shader("shaders\\test_shader.vs", "shaders\\test_shader.fs");
		testShader2 = new Shader("shaders\\test_shader.vs", "shaders\\test_shader.fs");
		testShader3 = new Shader("shaders\\test_shader.vs", "shaders\\test_shader.fs");
		testShader4 = new Shader("shaders\\test_shader.vs", "shaders\\test_shader.fs");
		initVAO();
		glEnable(GL_PROGRAM_POINT_SIZE);

		tex0 = loadTextureFromFile("textures\\container2.png");
		tex1 = loadTextureFromFile("textures\\container.jpg");

		testShader->use();
		testShader->setInt("texture1", 0);
		testShader->setInt("texture2", 1);

		// 将shader中的Matrices uniform块设置到0绑定点
		testShader->setBlockBinding("Matrices", 0);
		testShader2->setBlockBinding("Matrices", 0);
		testShader3->setBlockBinding("Matrices", 0);
		testShader4->setBlockBinding("Matrices", 0);

		initUBO();

		glEnable(GL_DEPTH_TEST);
	}

	void initVAO() {
		glGenVertexArrays(1, &VAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		float data[] = {
			1, 1,
			0, 0,
			-0.9, -0.8
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), &data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		glGenVertexArrays(1, &cubeVAO);
		unsigned int cubeVBO;
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, ccwVerticesSize, &ccwVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}

	void initUBO() {
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		/*
			glBindBufferRange将uniform缓冲对象绑定到绑定点上
			参数1，是目标缓冲的类型
			参数2，绑定点索引
			参数3，uniform缓冲对象
			参数4，偏移量
			参数5，大小
		*/
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(glm::mat4));
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), width / height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// 使用ubo的好处，view, project矩阵只需要设置一次
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		/*
		glBufferSubData填充缓冲的特定区域
		参数1，是目标缓冲的类型
		参数2，偏移量
		参数3，数据大小
		参数4，数据本身
		*/
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		

		// 绘制点
		/*glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 3);*/

		// 绘制立方体
		glBindVertexArray(cubeVAO);
		testShader->use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // 移动到左上角
		testShader->setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		testShader2->use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // 移动到左上角
		testShader2->setMat4("model", model);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		testShader3->use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // 移动到左上角
		testShader3->setMat4("model", model);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		testShader4->use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // 移动到左上角
		testShader4->setMat4("model", model);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		glBindVertexArray(0);
	}
	virtual void destroy() {
		delete(testShader);
		delete(testShader2);
		delete(testShader3);
		delete(testShader4);
	}
};