#include "program.h"

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern float vertices[];
extern unsigned int indices[];
extern int verticesSize;
extern int indicesSize;

Program initProgram() {
	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	// �ر���Ȳ���
	//glDisable(GL_DEPTH_TEST);

	Shader shader("..\\shader.vs", "..\\object_shader.fs");
	Shader lightShader("..\\shader.vs", "..\\light_shader.fs");

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// ��VAO
	glBindVertexArray(VAO);

	// ��VBO�����㻺������������GL_ARRAY_BUFFER�����´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

	

	// ��EBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

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

	// ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// ��������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int tex0 = loadTexture("..\\container2.png");
	unsigned int tex1 = loadTexture("..\\container2_specular.png");

	shader.use();
	//// glUniform1i���������������һ��λ��ֵ
	glUniform1i(glGetUniformLocation(shader.ID, "material.diffuse"), 0);
	shader.setInt("material.specular", 1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// ֻ��Ҫ��VBO��������Ҫ�ٴ�����VBO����Ϊ�������ӵ�VBO�Ѿ���������ȷ������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// ���õƵĶ������ԣ����ڵ���˵ֻ��λ�����ݣ�
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	// �����߿�ģʽ������1��ʾӦ�õ����������ε�����ͱ��棬����2��ʾ����������
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ���û�Ĭ��ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// ���VBO��������ΪglVertexAttribPointer�Ѿ���VBOע��Ϊ�������԰󶨵�VBO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ע�⣬��VAO����ʱ�����ܽ��EBO����ΪVAO������洢glBindBuffer�ĺ������ã�Ҳ��洢glBindBuffer�ĺ������ã�����VAO�оͲ�����EBO������
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ���VAO
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
	// ����1���������������洢�ں����unsigned int�����У�����ֻ�ǵ�����unsigned int
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
	}
	else {
		cerr << "failed to load texture" << endl;
	}

	stbi_image_free(data);
	return texture;
}

unsigned int loadTexture(char const* path) {
	unsigned int textureID;
	// ����1���������������洢�ں����unsigned int�����У�����ֻ�ǵ�����unsigned int
	glGenTextures(1, &textureID);

	// ����ͼƬ��nrChannels��ʾ��ɫͨ���ĸ���
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
		// ������֮���κε�����ָ��������õ�ǰ�󶨵�����
		glBindTexture(GL_TEXTURE_2D, textureID);
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

		// Ϊ��ǰ�󶨵�����������û��ƣ����˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // S, T, R �ȼ��� X, Y, Z
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