#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int createTexture(const char* fileName, unsigned int format, bool reverse = false) {
	unsigned int texture;
	// 生成1个纹理数量，并存储在后面的unsigned int数组中
	glGenTextures(1, &texture);
	// 绑定纹理，之后任何的纹理指令都可以配置当前绑定的纹理
	glBindTexture(GL_TEXTURE_2D, texture);

	// 为当前绑定的纹理对象设置环绕，过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 缩小过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // 放大过滤

	// 加载图片，nrChannels表示颜色通道的个数
	int width, height, nrChannels;
	if (reverse) {
		stbi_set_flip_vertically_on_load(true);
	}
	unsigned char* data = stbi_load(fileName, &width, &height, &nrChannels, 0);
	if (data) {
		/*
			生成一个纹理，调用了glTexImage2D以后，当前绑定的纹理对象就会被附加上纹理图像
			参数1，指定了纹理坐标，任何绑定了GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响
			参数2，为纹理指定多级渐远纹理的级别
			参数3，指定将纹理存储为何种格式
			参数4，设置最终纹理的宽度
			参数5，设置最终纹理的高度
			参数6，总是应该被设置为0，历史遗留问题
			参数7，定义了源图的格式
			参数8，定义了源图的数据类型
			参数9，指定了真正的图像数据
		*/
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		// 生成需要的多级渐远纹理
		glGenerateMipmap(GL_TEXTURE_2D);

		//glBindTexture(GL_TEXTURE_2D, texture);
	}
	else {
		cerr << "failed to load texture" << endl;
	}

	stbi_image_free(data);
	return texture;
}

void init(unsigned int* VBO, unsigned int* EBO, unsigned int* VAO, unsigned int* shaderProgram) {

	Shader myShader("..\\shader.vs", "..\\shader.fs");
	*shaderProgram = myShader.ID;
	//myShader.use();
	glUseProgram(myShader.ID);

	// 渲染三角形，三个顶点
	//float vertices[] = {
	//	-0.5f, -0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f,
	//	0.0f, 0.5f, 0.0f
	//};

	// 渲染四边形
	//float vertices[] = {
	//	0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 	1.0f, 1.0f,	// 右上角
	//	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 	1.0f, 0.0f,	// 右下角
	//	-0.5f, -0.5f, 0.0f,0.0f, 0.0f, 1.0f, 	0.0f, 0.0f,	// 左下角
	//	-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,	0.0f, 1.0f,	// 左上角
	//};

	// 渲染立方体
	//float vertices[] = {
	//	0.5f, 0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 	1.0f, 1.0f,	// 右上角
	//	0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 	1.0f, 0.0f,	// 右下角
	//	-0.5f, -0.5f, -0.5f,0.0f, 0.0f, 1.0f, 	0.0f, 0.0f,	// 左下角
	//	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,	0.0f, 1.0f,	// 左上角

	//	0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 	1.0f, 1.0f,	// 右上角
	//	0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 	1.0f, 0.0f,	// 右下角
	//	-0.5f, -0.5f, 0.5f,0.0f, 0.0f, 1.0f, 	0.0f, 0.0f,	// 左下角
	//	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,	0.0f, 1.0f,	// 左上角
	//};


	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// 三角形带颜色
	//float vertices[] = {
	//	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 
	//	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	//	0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
	//};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,

		0, 1, 5,
		0, 4, 5,

		4, 5, 6,
		4, 6, 7,

		7, 6, 3,
		2, 6, 3,

		0, 3, 4,
		3, 4, 7,

		1, 2, 5, 
		2, 5, 6,


	};
	
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	// 创建索引缓冲对象
	glGenBuffers(1, EBO);

	// 绑定VAO
	glBindVertexArray(*VAO);

	// 绑定VBO，顶点缓冲对象的类型是GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, *VBO); // 从这一刻起，所使用的任何缓冲调用都会用来配置当前绑定的VBO
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 绑定EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	// 以顶点属性位置值作为参数，启用顶点属性
	glEnableVertexAttribArray(0);


	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// 解绑VBO，允许，因为glVertexAttribPointer已经将VBO注册为顶点属性绑定的VBO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 注意，当VAO激活时，不能解绑EBO。因为VAO不仅会存储glBindBuffer的函数调用，也会存储glBindBuffer的函数调用，这样VAO中就不会有EBO配置了
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// 解绑VAO
	//glBindVertexArray(0);

	
	

	// 设置线框模式，参数1表示应用到所有三角形的正面和背面，参数2表示用线来绘制
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 设置会默认模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	unsigned int tex0 = createTexture("..\\container.jpg", GL_RGB);
	unsigned int tex1 = createTexture("..\\awesomeface.png", GL_RGBA, true);

	glUniform1i(glGetUniformLocation(myShader.ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(myShader.ID, "texture2"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	// 关闭深度测试
	//glDisable(GL_DEPTH_TEST);
}

// 摄像机位置
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
float yaw = -90.0f;  // 初始是-90.0f，是因为0度时，是指向右，所以向左旋转90度，使其指向前方
float pitch = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


float fov = 45;
void draw(unsigned int VAO, unsigned int shaderProgram) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	// 清除颜色缓冲和深度缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 view;
	// 参数1，相机位置，参数2，目标位置，参数3，向上的向量
	// cameraPos + cameraFront 保证无论摄像机位置是多少，摄像机一直是指向前方的
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// 模型矩阵
	//glm::mat4 model;
	////model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	// 观察矩阵
	//glm::mat4 view;
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
	// 投影矩阵
	glm::mat4 projection;
	/*
		构造一个透视投影矩阵
		参数1，指定了fov的值，表示的是视野
		参数2，设置了宽高比，由视口的宽除以高所得
		参数3，设置了近平面距离摄像机的距离
		参数4，设置了远平面距离摄像机的距离穿透
	*/
	projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

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

	//unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	/*
		参数1，uniform的位置值
		参数2，将要发送的矩阵的个数
		参数3，是否对矩阵进行置换（交换行和列）
		参数4，真正的矩阵数据
	*/
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	


	// 激活这个程序对象，激活后，每个着色器调用和渲染调用都会使用这个程序对象
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	//// 获取运行的秒数
	//float timeValue = glfwGetTime();
	//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	//// 查询uniform变量的位置值
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "cc");
	//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


	/*
		绘制函数
		参数1，指明打算绘制的OpenGL图元的类型
		参数2，指定了顶点数组的起始索引
		参数3，指定打算绘制多少个顶点
	*/
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 model;
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

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
}

// 清理资源
void deleteAllocate(unsigned int VBO, unsigned int EBO, unsigned int VAO, unsigned int shaderProgram) {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
}

int main() {
 	glfwInit();
	// 配置glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口对象，存放了所有和窗口相关的数据
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口的上下文设置为当前的上下文
	glfwMakeContextCurrent(window);
	// 注册函数，每当窗口大小调整时glfw会调用
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 给glad传入用来加载系统相关的OpenGL函数地址的函数
	// glfw提供的是glfwGetProcAddress，它根据编译的系统定义了正确的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// 设置隐藏光标，且光标不会离开窗口
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	unsigned int VBO, EBO, VAO, shaderProgram;
	init(&VBO, &EBO, &VAO, &shaderProgram);

	// 渲染循环，GLFW退出前一直运行
	while (!glfwWindowShouldClose(window)) {  // 循环每次开始前检查GLFW是否要退出

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 处理输入
		processInput(window);
		
		draw(VAO, shaderProgram);

		/*
			交换颜色缓冲
			它是一个存储着GLFW窗口每一个像素颜色值的大缓冲，它在这一迭代中被用来绘制，并作为输出显示在屏幕上
		*/
		glfwSwapBuffers(window);  
		glfwPollEvents();  // 检查有没有触发什么事件（比如键盘输入，锁表移动）
	}

	deleteAllocate(VBO, EBO, VAO, shaderProgram);

	// 释放之前分配的资源
	glfwTerminate();
	return 0;
}

// 每次窗口大小改变时调用，调整视口
// 当窗口第一次显示的时候也会被调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// 告诉OpenGL渲染窗口的大小，即视口，这样OpenGL才能知道怎样根据窗口大小显示数据和坐标
	// OpenGL的范围是-1到1，所以是将（-1到1）映射到（0, SCR_WIDTH），（0, SCR_HEIGHT）
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	// 按下esc时，关闭glfw，下次循环条件检测失败，会退出程序
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;  // 这里是相反的，y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.02f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// 根据偏航角计算方向向量
	glm::vec3 front;
	front.y = sin(glm::radians(pitch));
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}