#pragma once
#include "GLFW/glfw3.h"  // glfw可以帮助创建OpenGL上下文，定义窗口参数以及处理用户输入
#include "camera.h"
#include "shader.h"

class Scene {

public:
	virtual Camera* getCamera() { return new Camera(); };
	virtual void init(GLFWwindow* window, float width, float height) {  }
	virtual void update(GLFWwindow* window, float deltaTime) {}
	virtual void destroy() {}
};