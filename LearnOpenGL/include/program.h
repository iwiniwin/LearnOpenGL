#pragma once
#include "shader.h"

class Program {

public:
	//Shader shader;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int tex0;
	unsigned int tex1;
	unsigned int ID;
	
	void destory() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);

		glDeleteProgram(ID);
	}

private:

};

unsigned int createTexture(const char* fileName, unsigned int format, bool reverse = false);
Program initProgram();