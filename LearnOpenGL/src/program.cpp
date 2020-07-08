#include "program.h"

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern float vertices[];
extern unsigned int indices[];
extern int verticesSize;
extern int indicesSize;

Program initProgram() {
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	// 关闭深度测试
	//glDisable(GL_DEPTH_TEST);

	Shader shader("..\\shader.vs", "..\\object_shader.fs");
	Shader lightShader("..\\shader.vs", "..\\light_shader.fs");

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

	unsigned int tex0 = loadTexture("..\\container2.png");
	unsigned int tex1 = loadTexture("..\\container2_specular.png");

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
	//glBindVertexArray(0);

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

unsigned int createTexture(const char* fileName, unsigned int format, bool reverse) {
	unsigned int texture;
	// 生成1个纹理数量，并存储在后面的unsigned int数组中，这里只是单独的unsigned int
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
	}
	else {
		cerr << "failed to load texture" << endl;
	}

	stbi_image_free(data);
	return texture;
}

unsigned int loadTexture(char const* path) {
	unsigned int textureID;
	// 生成1个纹理数量，并存储在后面的unsigned int数组中，这里只是单独的unsigned int
	glGenTextures(1, &textureID);

	// 加载图片，nrChannels表示颜色通道的个数
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		// 绑定纹理，之后任何的纹理指令都可以配置当前绑定的纹理
		glBindTexture(GL_TEXTURE_2D, textureID);
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

		// 为当前绑定的纹理对象设置环绕，过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // S, T, R 等价于 X, Y, Z
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		cerr << "Texture failed to load at path : " << path << endl;
	}
	stbi_image_free(data);
	return textureID;
}