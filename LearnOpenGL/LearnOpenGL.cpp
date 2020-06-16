#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"  // �汾����
	"layout (location = 0) in vec3 aPos;\n"  // layout (location = 0) �趨�����ݱ�����λ��ֵ��ʹ��in�ؼ����������е����붥������
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  // gl_Position���õ�ֵ���Ϊ������ɫ�������
	"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"  // ʹ��out�ؼ��������������
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n"; 

void render() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// ��Ⱦ�����Σ���������
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};


	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// ���㻺������������GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // ����һ������ʹ�õ��κλ�����ö����������õ�ǰ�󶨵�VBO
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


	// ����һ��������ɫ������
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// ����ɫ��Դ�븽�ŵ���ɫ��������
	// ����2ָ���˴��ݵ�Դ���ַ�������
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// ������ɫ��
	glCompileShader(vertexShader);

	// �������Ƿ�ɹ�
	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	// ���ʧ�ܣ���ȡ������Ϣ
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}


	// ����һ��Ƭ����ɫ������
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &vertexShaderSource, NULL);
	glCompileShader(fragmentShader);

	/*
		��ɫ����������Ƕ����ɫ���ϲ�֮������������ɵİ汾�����Ҫʹ��֮ǰ�������ɫ����
		���Ǳ������������Ϊһ����ɫ���������Ȼ������Ⱦ�����ʱ�򼤻������ɫ������
	*/

	// ����һ����ɫ���������
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	// ��֮ǰ�������ɫ�����ӵ����������
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// ����
	glLinkProgram(shaderProgram);

	// ��������Ƿ�ɹ�
	int ret;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ret);
	// ���ʧ�ܣ���ȡ������Ϣ
	if (!ret) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	}

	// ����֮�󣬼ǵ�ɾ����ɫ�����󣬲�����Ҫ��
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// �������������󣬼����ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������
	glUseProgram(shaderProgram);
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
		// ��������
		processInput(window);
		
		render();

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

// ÿ�δ��ڴ�С�ı�ʱ���ã������ӿ�
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	// ����escʱ���ر�glfw���´�ѭ���������ʧ�ܣ����˳�����
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}