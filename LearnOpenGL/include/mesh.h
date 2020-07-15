#pragma once
#include "glm.hpp"
#include <string>
#include <vector>
#include "shader.h"
using namespace std;

// 顶点数据，C++结构体在内存布局上是连续的
struct Vertex {
	glm::vec3 Position;  // 顶点坐标
	glm::vec3 Normal;	// 顶点法线
	glm::vec2 TexCoords;  // 纹理坐标
};

// 纹理数据
struct Texture {
	unsigned int id;  // 纹理ID
	string type;  // 纹理类型，是漫反射贴图或镜面光贴图
	string path;  // 存储纹理的路径，用于和其他纹理比较
};

class Mesh {
	
public:
	vector<Vertex> vertices;
	vector<Texture> textures;
	vector<unsigned int> indices;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
		this->vertices = vertices;
		this->textures = textures;
		this->indices = indices;

		setupMesh();
	}

	void draw(Shader shader) {
		// 绑定相应的纹理
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = to_string(specularNr++);
			else if (name == "texture_normal")
				number = to_string(normalNr++);
			else if (name == "texture_height")
				number = to_string(heightNr++);
			shader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		
		// 绘制网格
		glBindVertexArray(VAO);
		/*
			指明从索引缓冲渲染，该函数从当前绑定到的GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取索引
			参数1，指明绘制的OpenGL图元的类型
			参数2，指定打算绘制顶点的个数
			参数3，是索引的类型
			参数4，指定EBO中的偏移量
		*/
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
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
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// 顶点位置
		glEnableVertexAttribArray(0);
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// 顶点法线
		glEnableVertexAttribArray(1);
		// offsetof预处理指令，会返回第二个参数距结构体头部的字节偏移量
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// 纹理坐标
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// 解绑
		glBindVertexArray(0);
	}
};