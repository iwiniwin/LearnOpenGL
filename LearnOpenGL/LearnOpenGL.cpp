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
	"layout (location = 1) in vec3 cc;\n"
	"out vec3 our;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  // gl_Position设置的值会成为顶点着色器的输出
	"	our = cc;\n"
	"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"  // 使用out关键字声明输出变量
	"in vec3 our;\n"
	"void main()\n"
	"{\n"
	"	// FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"	FragColor = vec4(our, 1.0);\n"
	"}\n"; 

void init(unsigned int* VBO, unsigned int* EBO, unsigned int* VAO, unsigned int* shaderProgram) {


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
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	/*
		着色器程序对象是多个着色器合并之后并最终链接完成的版本，如果要使用之前编译的着色器，
		我们必须把它们链接为一个着色器程序对象，然后在渲染对象的时候激活这个着色器程序
	*/

	// 创建一个着色器程序对象
	*shaderProgram = glCreateProgram();
	// 将之前编译的着色器附加到程序对象上
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragmentShader);
	// 链接
	glLinkProgram(*shaderProgram);

	// 检测链接是否成功
	int ret;
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &ret);
	// 如果失败，获取错误信息
	if (!ret) {
		char infoLog[512];
		glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
	}

	// 链接之后，记得删除着色器对象，不再需要了
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 渲染三角形，三个顶点
	//float vertices[] = {
	//	-0.5f, -0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f,
	//	0.0f, 0.5f, 0.0f
	//};

	// 渲染四边形
	//float vertices[] = {
	//	0.5f, 0.5f, 0.0f,		// 右上角
	//	0.5f, -0.5f, 0.0f,		// 右下角
	//	-0.5f, -0.5f, 0.0f,		// 左下角
	//	-0.5f, 0.5f, 0.0f,		// 左上角
	//};

	// 三角形带颜色
	float vertices[] = {
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	// 以顶点属性位置值作为参数，启用顶点属性
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 解绑VBO，允许，因为glVertexAttribPointer已经将VBO注册为顶点属性绑定的VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 注意，当VAO激活时，不能解绑EBO。因为VAO不仅会存储glBindBuffer的函数调用，也会存储glBindBuffer的函数调用，这样VAO中就不会有EBO配置了
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// 解绑VAO
	glBindVertexArray(0);

	
	

	// 设置线框模式，参数1表示应用到所有三角形的正面和背面，参数2表示用线来绘制
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 设置会默认模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw(unsigned int VAO, unsigned int shaderProgram) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

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
	glDrawArrays(GL_TRIANGLES, 0, 3);

	/*
		指明从索引缓冲渲染，该函数从当前绑定到的GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取索引
		参数1，指明绘制的OpenGL图元的类型
		参数2，指定打算绘制顶点的个数
		参数3，是索引的类型
		参数4，指定EBO中的偏移量
	*/
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

	unsigned int VBO, EBO, VAO, shaderProgram;
	init(&VBO, &EBO, &VAO, &shaderProgram);

	// 渲染循环，GLFW退出前一直运行
	while (!glfwWindowShouldClose(window)) {  // 循环每次开始前检查GLFW是否要退出
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
}