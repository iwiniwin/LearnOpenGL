#include "program.h"

#include "shader.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


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

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		cerr << "Texture failed to load at path : " << path << endl;
	}
	stbi_image_free(data);
	return textureID;
}