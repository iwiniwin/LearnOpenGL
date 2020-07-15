#pragma once
#include "scene.h"
#include "model.h"

extern float vertices[];
extern unsigned int indices[];
extern int verticesSize;
extern int indicesSize;

class MultipleLightScene : Scene {

public:
	void init(GLFWwindow* window, float width, float height) {
		// 开启深度测试
		glEnable(GL_DEPTH_TEST);
		// 关闭深度测试
		//glDisable(GL_DEPTH_TEST);

		Shader shader("shaders\\shader.vs", "shaders\\object_shader.fs");
		Shader lightShader("shaders\\shader.vs", "shaders\\light_shader.fs");

		unsigned int VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		// 绑定VAO
		glBindVertexArray(VAO);

		// 绑定VBO，顶点缓冲对象的类型是GL_ARRAY_BUFFER，将新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		/*
			glBufferData专门用来把用户定义的数据复制到当前绑定缓冲
			参数1，是目标缓冲的类型
			参数2，指定传输数据的大小（以字节为单位）
			参数3，是希望发送的实际数据
			参数4，指定了显卡如何管理给定的数据
				GL_STATIC_DRAW : 数据不会或几乎不会被改变
				GL_DYNAMIC_DRAW : 数据会被改变很多
				GL_STREAM_DRAW : 数据每次绘制时都会改变
		*/
		glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);



		// 绑定EBO
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

		/*
			设置顶点属性指针
			告诉OpenGL如何解析顶点数据，如何把顶点数据链接到顶点着色器的顶点属性上
			参数1，指定要配置的顶点属性，对应顶点着色器中的layout(location = 0)
			参数2，指定顶点属性的大小，这里顶点属性是vec3，由3个值组成
			参数3，指定数据的类型
			参数4，定义是否希望数据被标准化，如果设置为GL_TRUE，所有数据都会被映射到0（对于有符号数据是-1）到1之间
			参数5，指定步长stride，说明连续的顶点属性组之间的间隔，简单说就是整个属性第二次出现的地方到整个数组0位置之间有多少字节
			参数6，表示位置数据在缓冲中起始位置的偏移量
		*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		// 以顶点属性位置值作为参数，启用顶点属性
		glEnableVertexAttribArray(0);

		// 法线
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// 纹理坐标
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		unsigned int tex0 = loadTextureFromFile("textures\\container2.png");
		unsigned int tex1 = loadTextureFromFile("textures\\container2_specular.png");

		shader.use();
		//// glUniform1i给纹理采样器分配一个位置值
		glUniform1i(glGetUniformLocation(shader.ID, "material.diffuse"), 0);
		shader.setInt("material.specular", 1);

		unsigned int lightVAO;
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);
		// 只需要绑定VBO，而不需要再次设置VBO，因为上面箱子的VBO已经设置了正确的数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 设置灯的顶点属性（对于灯来说只有位置数据）
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		// 设置线框模式，参数1表示应用到所有三角形的正面和背面，参数2表示用线来绘制
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// 设置回默认模式
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// 解绑VBO，允许，因为glVertexAttribPointer已经将VBO注册为顶点属性绑定的VBO
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		// 注意，当VAO激活时，不能解绑EBO。因为VAO不仅会存储glBindBuffer的函数调用，也会存储glBindBuffer的函数调用，这样VAO中就不会有EBO配置了
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// 解绑VAO
		glBindVertexArray(0);

		Program program;

		program.VAO = VAO;
		program.LightVAO = lightVAO;
		program.VBO = VBO;
		program.EBO = EBO;
		program.ID = shader.ID;
		program.LightID = lightShader.ID;
		program.tex0 = tex0;
		program.tex1 = tex1;

		return program;
	}
	void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// 清除颜色缓冲和深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 激活纹理单元
		glActiveTexture(GL_TEXTURE0);
		// 绑定这个纹理到当前激活的纹理单元
		glBindTexture(GL_TEXTURE_2D, program.tex0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, program.tex1);

		// 灯的位置
		glm::vec3 lightPos(1.0f, 0.4f, 1.0f);
		// 光源方向
		glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);

		glm::vec3 lightColor(1.0f);
		//lightColor.x = sin(glfwGetTime() * 2.0f);
		//lightColor.y = sin(glfwGetTime() * 0.7f);
		//lightColor.z = sin(glfwGetTime() * 1.3f);

		// 激活这个程序对象，激活后，每个着色器调用和渲染调用都会使用这个程序对象
		glUseProgram(program.ID);
		glUniform3fv(glGetUniformLocation(program.ID, "viewPos"), 1, &camera.Position[0]);

		glUniform3f(glGetUniformLocation(program.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(program.ID, "material.shininess"), 64.0f);

		// 平行光
		glUniform3fv(glGetUniformLocation(program.ID, "dirLight.direction"), 1, &lightDirection[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "dirLight.ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "dirLight.diffuse"), 1, &(lightColor * glm::vec3(0.4f))[0]);
		glUniform3f(glGetUniformLocation(program.ID, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		// 点光源
		glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[0].position"), 1, &pointLightPositions[0][0]);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[0].quadratic"), 0.032f);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[0].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[0].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
		glUniform3f(glGetUniformLocation(program.ID, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);

		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[1].position"), 1, &pointLightPositions[1][0]);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[1].quadratic"), 0.032f);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[1].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[1].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
		glUniform3f(glGetUniformLocation(program.ID, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);

		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[2].position"), 1, &pointLightPositions[2][0]);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[2].quadratic"), 0.032f);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[2].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[2].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
		glUniform3f(glGetUniformLocation(program.ID, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);

		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[3].position"), 1, &pointLightPositions[3][0]);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(program.ID, "pointLights[3].quadratic"), 0.032f);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[3].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "pointLights[3].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
		glUniform3f(glGetUniformLocation(program.ID, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);

		// 聚光
		glUniform3fv(glGetUniformLocation(program.ID, "spotLight.position"), 1, &camera.Position[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "spotLight.direction"), 1, &camera.Front[0]);
		glUniform1f(glGetUniformLocation(program.ID, "spotLight.cutoff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(program.ID, "spotLight.outercutoff"), glm::cos(glm::radians(15.0f)));
		glUniform3fv(glGetUniformLocation(program.ID, "spotLight.ambient"), 1, &(lightColor * glm::vec3(0.0f))[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "spotLight.diffuse"), 1, &(lightColor * glm::vec3(1.0f))[0]);
		glUniform3f(glGetUniformLocation(program.ID, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(program.ID, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(program.ID, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(program.ID, "spotLight.quadratic"), 0.032f);



		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		/*
			参数1，uniform的位置值
			参数2，将要发送的矩阵的个数
			参数3，是否对矩阵进行置换（交换行和列）
			参数4，真正的矩阵数据
		*/
		//glUniformMatrix4fv(glGetUniformLocation(program.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view;
		// 参数1，相机位置，参数2，目标位置，参数3，向上的向量
		// cameraPos + cameraFront 保证无论摄像机位置是多少，摄像机一直是指向前方的
		view = camera.GetViewMatrix();
		unsigned int viewLoc = glGetUniformLocation(program.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// 投影矩阵
		glm::mat4 projection;
		/*
			构造一个透视投影矩阵
			参数1，指定了fov的值，表示的是视野
			参数2，设置了宽高比，由视口的宽除以高所得
			参数3，设置了近平面距离摄像机的距离
			参数4，设置了远平面距离摄像机的距离穿透
		*/
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		unsigned int projectionLoc = glGetUniformLocation(program.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));



		glBindVertexArray(program.VAO);
		/*
			绘制函数
			参数1，指明打算绘制的OpenGL图元的类型
			参数2，指定了顶点数组的起始索引
			参数3，指定打算绘制多少个顶点
		*/
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(program.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		/*
			指明从索引缓冲渲染，该函数从当前绑定到的GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取索引
			参数1，指明绘制的OpenGL图元的类型
			参数2，指定打算绘制顶点的个数
			参数3，是索引的类型
			参数4，指定EBO中的偏移量
		*/
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glUseProgram(program.LightID);
		glUniformMatrix4fv(glGetUniformLocation(program.LightID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(program.LightID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(program.LightVAO);
		for (int i = 0; i < 4; i++) {
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, pointLightPositions[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.04f));
			glUniformMatrix4fv(glGetUniformLocation(program.LightID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	void destroy() {

	}
};