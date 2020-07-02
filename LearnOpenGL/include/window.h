#pragma once
#include "GLFW/glfw3.h"  // glfw���԰�������OpenGL�����ģ����崰�ڲ����Լ������û�����

typedef void(*Update)(GLFWwindow* window, float);
typedef void(*OnDestroy)();

GLFWwindow* initWindow();
void render_loop(GLFWwindow* window, Update update, OnDestroy onDestroy);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


