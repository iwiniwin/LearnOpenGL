#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>  // ����glad����ȡ���еı���OpenGLͷ�ļ�
#include <glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader {
public:
	// ����ID
	unsigned int ID;

	// ��������ȡ��������ɫ��
	Shader(const GLchar* vertexPath, const GLchar* fragmentpath, const GLchar* geometryPath = nullptr) {
		// ���ļ�·���л�ȡ����/Ƭ����ɫ��
		string vertextCode, fragmentCode, geometryCode;
		ifstream vShaderFile, fShaderFile, gShaderFile;
		// �����쳣���룬ָ����Щ״̬��־ʱ�׳��쳣
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			// ���ļ�
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentpath);
			stringstream vShaderStream, fShaderStream;
			// ��ȡ�ļ��Ļ������ݵ���������
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			
			vShaderFile.close();
			fShaderFile.close();
			// ת����������str
			vertextCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

			if (geometryPath != nullptr) {
				gShaderFile.open(geometryPath);
				stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (ifstream::failure e) {
			cerr << "read shader file error : \n" << e.what() << endl;
		}
		compileShader(vertextCode.c_str(), fragmentCode.c_str(), geometryPath == nullptr ? nullptr : geometryCode.c_str());
	}


	// �������
	void use() {
		glUseProgram(ID);
	}

	// uniform���ߺ���
	void setBool(const string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec3(const string& name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec3(const string& name, const float x, const float y, const float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec4(const string& name, const glm::vec4& value) {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec4(const string& name, float x, float y, float z, float, float w) {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const string& name, const glm::mat2& mat) {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const string& name, const glm::mat3& mat) {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void  setMat4(const string& name, const glm::mat4& mat) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setBlockBinding(const string& name, int value) {
		glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), value);
	}

private:

	void compileShader(const char* vShaderCode, const char* fShaderCode, const char* gShaderCode) {
		unsigned int vertex, fragment, geometry;
		int success;
		char infoLog[512];
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			cerr << "vertex shader error : \n" << infoLog << endl;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			cerr << "fragment shader error : \n" << infoLog << endl;
		}

		if (gShaderCode != nullptr) {
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(geometry, 512, NULL, infoLog);
				cerr << "geometry shader error : \n" << infoLog << endl;
			}
		}

		// ������ɫ���������
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (gShaderCode != nullptr) {
			glAttachShader(ID, geometry);
		}
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			cerr << "program link error : \n" << infoLog << endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (gShaderCode != nullptr) {
			glDeleteShader(geometry);
		}
	}

};

#endif