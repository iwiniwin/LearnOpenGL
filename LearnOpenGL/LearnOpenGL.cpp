#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 每次窗口大小改变时调用，调整视口
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

	glfwInit();
	// 配置glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口对象，存放了所有和窗口相关的数据
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口的上下文设置为当前的上下文
	glfwMakeContextCurrent(window);

	// 给glad传入用来加载系统相关的OpenGL函数地址的函数
	// glfw提供的是glfwGetProcAddress，它根据编译的系统定义了正确的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// 告诉OpenGL渲染窗口的大小，即视口，这样OpenGL才能知道怎样根据窗口大小显示数据和坐标
	// OpenGL的范围是-1到1，所以是将（-1到1）映射到（0, 800），（0, 600）
	glViewport(0, 0, 800, 600);

	// 注册函数，每当窗口大小调整时glfw会调用
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 渲染循环，GLFW退出前一直运行
	while (!glfwWindowShouldClose(window)) {  // 循环每次开始前检查GLFW是否要退出
		/*
			交换颜色缓冲
			它是一个存储着GLFW窗口每一个像素颜色值的大缓冲，它在这一迭代中被用来绘制，并作为输出显示在屏幕上
		*/
		glfwSwapBuffers(window);  
		glfwPollEvents();  // 检查有没有触发什么事件（比如键盘输入，锁表移动）
	}

	// 释放之前分配的资源
	glfwTerminate();
	return 0;
}