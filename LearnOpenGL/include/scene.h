#pragma once
#include "GLFW/glfw3.h"  // glfw���԰�������OpenGL�����ģ����崰�ڲ����Լ������û�����
#include "camera.h"
#include "shader.h"

class Scene {

public:
	virtual Camera* getCamera() { return new Camera(); };
	virtual void init(GLFWwindow* window, float width, float height) {  }
	virtual void update(GLFWwindow* window, float deltaTime) {}
	virtual void destroy() {}
};