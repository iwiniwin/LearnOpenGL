#include "program.h"

#include "shader.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


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

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		cerr << "Texture failed to load at path : " << path << endl;
	}
	stbi_image_free(data);
	return textureID;
}