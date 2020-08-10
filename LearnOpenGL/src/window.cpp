#include "glad/glad.h"	// glad��������OpenGL����ָ��
#include "window.h"
#include <iostream>
using namespace std;

// settings
extern const unsigned int SCR_WIDTH = 800;
extern const unsigned int SCR_HEIGHT = 600;

GLFWwindow* initWindow() {
	// ��ʼ��glfw
	glfwInit();
	// ����glfw
	// ����glfwʹ�õ�OpenGL��3.3�汾
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// ����glfwʹ��OpenGL����ģʽ
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);  // ʹ��һ������4�������������㣩�Ķ��ز�������

	// ����һ�����ڶ�����������������кʹ�����ص�����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return NULL;
	}
	// ����������Ϊ����Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);

	// ��glad������������ϵͳ��ص�OpenGL����ָ���ַ�ĺ���
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

		// ��鲢�����¼�
		glfwPollEvents();
		/*
			������ɫ����
			����һ���洢��GLFW����ÿһ��������ɫֵ�Ĵ󻺳壬������һ�����б��������ƣ�����Ϊ�����ʾ����Ļ��
		*/
		glfwSwapBuffers(window);
	}
	
	onDestroy();

	// �ͷ�֮ǰ�������Դ
	glfwTerminate();
}


// ÿ�δ��ڴ�С�ı�ʱ���ã������ӿ�
// �����ڵ�һ����ʾ��ʱ��Ҳ�ᱻ����
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// ����OpenGL��Ⱦ���ڵĳߴ��С�����ӿڣ�����OpenGL����֪���������ݴ��ڴ�С��ʾ���ݺ�����
	// OpenGL�ķ�Χ��-1��1�������ǽ���-1��1��ӳ�䵽��0, width������0, height��
	// ǰ�����������ƴ������½ǵ�λ��
	glViewport(0, 0, width, height);
}