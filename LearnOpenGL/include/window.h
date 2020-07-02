#pragma once
#include "GLFW/glfw3.h"  // glfw可以帮助创建OpenGL上下文，定义窗口参数以及处理用户输入

typedef void(*Update)(GLFWwindow* window, float);
typedef void(*OnDestroy)();

GLFWwindow* initWindow();
void render_loop(GLFWwindow* window, Update update, OnDestroy onDestroy);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


