#include "glad/glad.h"	// glad用来管理OpenGL函数指针
#include "window.h"
#include <iostream>
using namespace std;

// settings
extern const unsigned int SCR_WIDTH = 800;
extern const unsigned int SCR_HEIGHT = 600;

GLFWwindow* initWindow() {
	// 初始化glfw
	glfwInit();
	// 配置glfw
	// 告诉glfw使用的OpenGL是3.3版本
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// 告诉glfw使用OpenGL核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);  // 使用一个包含4个样本（采样点）的多重采样缓冲

	// 创建一个窗口对象，这个对象存放了所有和窗口相关的数据
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return NULL;
	}
	// 将窗口上限为设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	// 给glad传入用来加载系统相关的OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return NULL;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

float lastFrame = 0.0f;
float deltaTime = 0.0f;
void render_loop(GLFWwindow* window, Update update, OnDestroy onDestroy) {
	while (!glfwWindowShouldClose(window)) {
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		update(window, deltaTime);

		// 检查并调用事件
		glfwPollEvents();
		/*
			交换颜色缓冲
			它是一个存储着GLFW窗口每一个像素颜色值的大缓冲，它在这一迭代中被用来绘制，并作为输出显示在屏幕上
		*/
		glfwSwapBuffers(window);
	}
	
	onDestroy();

	// 释放之前分配的资源
	glfwTerminate();
}


// 每次窗口大小改变时调用，调整视口
// 当窗口第一次显示的时候也会被调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// 告诉OpenGL渲染窗口的尺寸大小，即视口，这样OpenGL才能知道怎样根据窗口大小显示数据和坐标
	// OpenGL的范围是-1到1，所以是将（-1到1）映射到（0, width），（0, height）
	// 前两个参数控制窗口左下角的位置
	glViewport(0, 0, width, height);
}