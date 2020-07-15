#pragma once
#include "glm.hpp"
#include <string>
#include <vector>
#include "shader.h"
using namespace std;

// �������ݣ�C++�ṹ�����ڴ沼������������
struct Vertex {
	glm::vec3 Position;  // ��������
	glm::vec3 Normal;	// ���㷨��
	glm::vec2 TexCoords;  // ��������
};

// ��������
struct Texture {
	unsigned int id;  // ����ID
	string type;  // �������ͣ�����������ͼ�������ͼ
	string path;  // �洢�����·�������ں���������Ƚ�
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
		// ����Ӧ������
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
		
		// ��������
		glBindVertexArray(VAO);
		/*
			ָ��������������Ⱦ���ú����ӵ�ǰ�󶨵���GL_ELEMENT_ARRAY_BUFFERĿ���EBO�л�ȡ����
			����1��ָ�����Ƶ�OpenGLͼԪ������
			����2��ָ��������ƶ���ĸ���
			����3��������������
			����4��ָ��EBO�е�ƫ����
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
			glBufferDataר���������û���������ݸ��Ƶ���ǰ�󶨻���
			����1����Ŀ�껺�������
			����2��ָ���������ݵĴ�С�����ֽ�Ϊ��λ��
			����3����ϣ�����͵�ʵ������
			����4��ָ�����Կ���ι������������
				GL_STATIC_DRAW : ���ݲ���򼸺����ᱻ�ı�
				GL_DYNAMIC_DRAW : ���ݻᱻ�ı�ܶ�
				GL_STREAM_DRAW : ����ÿ�λ���ʱ����ı�
		*/
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// ����λ��
		glEnableVertexAttribArray(0);
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// ���㷨��
		glEnableVertexAttribArray(1);
		// offsetofԤ����ָ��᷵�صڶ���������ṹ��ͷ�����ֽ�ƫ����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// ��������
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// ���
		glBindVertexArray(0);
	}
};