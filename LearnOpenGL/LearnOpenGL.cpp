#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ÿ�δ��ڴ�С�ı�ʱ���ã������ӿ�
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

	glfwInit();
	// ����glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// �������ڶ��󣬴�������кʹ�����ص�����
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	// �����ڵ�����������Ϊ��ǰ��������
	glfwMakeContextCurrent(window);

	// ��glad������������ϵͳ��ص�OpenGL������ַ�ĺ���
	// glfw�ṩ����glfwGetProcAddress�������ݱ����ϵͳ��������ȷ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// ����OpenGL��Ⱦ���ڵĴ�С�����ӿڣ�����OpenGL����֪���������ݴ��ڴ�С��ʾ���ݺ�����
	// OpenGL�ķ�Χ��-1��1�������ǽ���-1��1��ӳ�䵽��0, 800������0, 600��
	glViewport(0, 0, 800, 600);

	// ע�ắ����ÿ�����ڴ�С����ʱglfw�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��Ⱦѭ����GLFW�˳�ǰһֱ����
	while (!glfwWindowShouldClose(window)) {  // ѭ��ÿ�ο�ʼǰ���GLFW�Ƿ�Ҫ�˳�
		/*
			������ɫ����
			����һ���洢��GLFW����ÿһ��������ɫֵ�Ĵ󻺳壬������һ�����б��������ƣ�����Ϊ�����ʾ����Ļ��
		*/
		glfwSwapBuffers(window);  
		glfwPollEvents();  // �����û�д���ʲô�¼�������������룬�����ƶ���
	}

	// �ͷ�֮ǰ�������Դ
	glfwTerminate();
	return 0;
}