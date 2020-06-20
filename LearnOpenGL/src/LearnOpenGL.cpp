#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void loadTexture(const char* fileName) {

}

unsigned int createTexture(const char* fileName, unsigned int format, bool reverse = false) {
	unsigned int texture;
	// ����1���������������洢�ں����unsigned int������
	glGenTextures(1, &texture);
	// ������֮���κε�����ָ��������õ�ǰ�󶨵�����
	glBindTexture(GL_TEXTURE_2D, texture);

	// Ϊ��ǰ�󶨵�����������û��ƣ����˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // ��С����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // �Ŵ����

	// ����ͼƬ��nrChannels��ʾ��ɫͨ���ĸ���
	int width, height, nrChannels;
	if (reverse) {
		stbi_set_flip_vertically_on_load(true);
	}
	unsigned char* data = stbi_load(fileName, &width, &height, &nrChannels, 0);
	if (data) {
		/*
			����һ������������glTexImage2D�Ժ󣬵�ǰ�󶨵��������ͻᱻ����������ͼ��
			����1��ָ�����������꣬�κΰ���GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ��
			����2��Ϊ����ָ���༶��Զ����ļ���
			����3��ָ��������洢Ϊ���ָ�ʽ
			����4��������������Ŀ��
			����5��������������ĸ߶�
			����6������Ӧ�ñ�����Ϊ0����ʷ��������
			����7��������Դͼ�ĸ�ʽ
			����8��������Դͼ����������
			����9��ָ����������ͼ������
		*/
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		// ������Ҫ�Ķ༶��Զ����
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

	// ��Ⱦ�����Σ���������
	//float vertices[] = {
	//	-0.5f, -0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f,
	//	0.0f, 0.5f, 0.0f
	//};

	// ��Ⱦ�ı���
	float vertices[] = {
		0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 	1.0f, 1.0f,	// ���Ͻ�
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 	1.0f, 0.0f,	// ���½�
		-0.5f, -0.5f, 0.0f,0.0f, 0.0f, 1.0f, 	0.0f, 0.0f,	// ���½�
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,	0.0f, 1.0f,	// ���Ͻ�
	};

	// �����δ���ɫ
	//float vertices[] = {
	//	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 
	//	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	//	0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
	//};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
	};
	
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	// ���������������
	glGenBuffers(1, EBO);

	// ��VAO
	glBindVertexArray(*VAO);

	// ��VBO�����㻺������������GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, *VBO); // ����һ������ʹ�õ��κλ�����ö����������õ�ǰ�󶨵�VBO
	/*
		glBufferDataר���������û���������ݸ��Ƶ���ǰ�󶨻���
		����1����Ŀ�껺�������
		����2��ָ���������ݵĴ�С�����ֽ�Ϊ��λ��
		����3����ϣ�����͵�ʵ������
		����4��ָ�����Կ���ι������������
			GL_STATIC_DRAW : ���ݲ���򼸺����ᱻ�ı�
			GL_DYNAMIC_DRAW : ���ݻᱻ�ı�ܶ�
			GL_STREAM_DRAW : ����ÿ�λ���ʱ����ı�
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ��EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/*
		���ö�������ָ��
		����OpenGL��ν����������ݣ���ΰѶ����������ӵ�������ɫ���Ķ���������
		����1��ָ��Ҫ���õĶ������ԣ���Ӧ������ɫ���е�layout(location = 0)
		����2��ָ���������ԵĴ�С�����ﶥ��������vec3����3��ֵ���
		����3��ָ�����ݵ�����
		����4�������Ƿ�ϣ�����ݱ���׼�����������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з���������-1����1֮��
		����5��ָ������stride��˵�������Ķ���������֮��ļ������˵�����������Եڶ��γ��ֵĵط�����������0λ��֮���ж����ֽ�
		����6����ʾλ�������ڻ�������ʼλ�õ�ƫ����
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// �Զ�������λ��ֵ��Ϊ���������ö�������
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// ���VBO��������ΪglVertexAttribPointer�Ѿ���VBOע��Ϊ�������԰󶨵�VBO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ע�⣬��VAO����ʱ�����ܽ��EBO����ΪVAO������洢glBindBuffer�ĺ������ã�Ҳ��洢glBindBuffer�ĺ������ã�����VAO�оͲ�����EBO������
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ���VAO
	//glBindVertexArray(0);

	
	

	// �����߿�ģʽ������1��ʾӦ�õ����������ε�����ͱ��棬����2��ʾ����������
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ���û�Ĭ��ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	unsigned int tex0 = createTexture("..\\container.jpg", GL_RGB);
	unsigned int tex1 = createTexture("..\\awesomeface.png", GL_RGBA, true);

	glUniform1i(glGetUniformLocation(myShader.ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(myShader.ID, "texture2"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
}

void draw(unsigned int VAO, unsigned int shaderProgram) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// �������������󣬼����ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	//// ��ȡ���е�����
	//float timeValue = glfwGetTime();
	//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	//// ��ѯuniform������λ��ֵ
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "cc");
	//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


	/*
		���ƺ���
		����1��ָ��������Ƶ�OpenGLͼԪ������
		����2��ָ���˶����������ʼ����
		����3��ָ��������ƶ��ٸ�����
	*/
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	/*
		ָ��������������Ⱦ���ú����ӵ�ǰ�󶨵���GL_ELEMENT_ARRAY_BUFFERĿ���EBO�л�ȡ����
		����1��ָ�����Ƶ�OpenGLͼԪ������
		����2��ָ��������ƶ���ĸ���
		����3��������������
		����4��ָ��EBO�е�ƫ����
	*/
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// ������Դ
void deleteAllocate(unsigned int VBO, unsigned int EBO, unsigned int VAO, unsigned int shaderProgram) {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
}

int main() {
 	glfwInit();
	// ����glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// �������ڶ��󣬴�������кʹ�����ص�����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	// �����ڵ�����������Ϊ��ǰ��������
	glfwMakeContextCurrent(window);
	// ע�ắ����ÿ�����ڴ�С����ʱglfw�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��glad������������ϵͳ��ص�OpenGL������ַ�ĺ���
	// glfw�ṩ����glfwGetProcAddress�������ݱ����ϵͳ��������ȷ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	unsigned int VBO, EBO, VAO, shaderProgram;
	init(&VBO, &EBO, &VAO, &shaderProgram);

	// ��Ⱦѭ����GLFW�˳�ǰһֱ����
	while (!glfwWindowShouldClose(window)) {  // ѭ��ÿ�ο�ʼǰ���GLFW�Ƿ�Ҫ�˳�
		// ��������
		processInput(window);
		
		draw(VAO, shaderProgram);

		/*
			������ɫ����
			����һ���洢��GLFW����ÿһ��������ɫֵ�Ĵ󻺳壬������һ�����б��������ƣ�����Ϊ�����ʾ����Ļ��
		*/
		glfwSwapBuffers(window);  
		glfwPollEvents();  // �����û�д���ʲô�¼�������������룬�����ƶ���
	}

	deleteAllocate(VBO, EBO, VAO, shaderProgram);

	// �ͷ�֮ǰ�������Դ
	glfwTerminate();
	return 0;
}

// ÿ�δ��ڴ�С�ı�ʱ���ã������ӿ�
// �����ڵ�һ����ʾ��ʱ��Ҳ�ᱻ����
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// ����OpenGL��Ⱦ���ڵĴ�С�����ӿڣ�����OpenGL����֪���������ݴ��ڴ�С��ʾ���ݺ�����
	// OpenGL�ķ�Χ��-1��1�������ǽ���-1��1��ӳ�䵽��0, SCR_WIDTH������0, SCR_HEIGHT��
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	// ����escʱ���ر�glfw���´�ѭ���������ʧ�ܣ����˳�����
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}