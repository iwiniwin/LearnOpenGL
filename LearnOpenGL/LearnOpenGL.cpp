#include <iostream>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"  // 版本声明
	"layout (location = 0) in vec3 aPos;\n"  // layout (location = 0) 设定了数据变量的位置值，使用in关键字声明所有的输入顶点属性
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  // gl_Position设置的值会成为顶点着色器的输出
	"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"  // 使用out关键字声明输出变量
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n"; 

void render() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 渲染三角形，三个顶点
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};


	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// 顶点缓冲对象的类型是GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // 从这一刻起，所使用的任何缓冲调用都会用来配置当前绑定的VBO
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


	// 创建一个顶点着色器对象
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// 把着色器源码附着到着色器对象上
	// 参数2指定了传递的源码字符串数量
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// 编译着色器
	glCompileShader(vertexShader);

	// 检测编译是否成功
	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	// 如果失败，获取错误信息
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}


	// 创建一个片段着色器对象
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &vertexShaderSource, NULL);
	glCompileShader(fragmentShader);

	/*
		着色器程序对象是多个着色器合并之后并最终链接完成的版本，如果要使用之前编译的着色器，
		我们必须把它们链接为一个着色器程序对象，然后在渲染对象的时候激活这个着色器程序
	*/

	// 创建一个着色器程序对象
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	// 将之前编译的着色器附加到程序对象上
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// 链接
	glLinkProgram(shaderProgram);

	// 检测链接是否成功
	int ret;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ret);
	// 如果失败，获取错误信息
	if (!ret) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	}

	// 链接之后，记得删除着色器对象，不再需要了
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 激活这个程序对象，激活后，每个着色器调用和渲染调用都会使用这个程序对象
	glUseProgram(shaderProgram);
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
		// 处理输入
		processInput(window);
		
		render();

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

// 每次窗口大小改变时调用，调整视口
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	// 按下esc时，关闭glfw，下次循环条件检测失败，会退出程序
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}