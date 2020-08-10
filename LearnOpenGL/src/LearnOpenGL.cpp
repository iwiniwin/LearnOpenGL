#include <iostream>
using namespace std;
#include "camera.h"
#include "window.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "scene.h"

#include "scenes/model_scene.h"
#include "scenes/multiple_light_scene.h"
#include "scenes/depth_scene.h"
#include "scenes/stencil_scene.h"
#include "scenes/blend_scene.h"
#include "scenes/framebuffer_scene.h"
#include "scenes/cubemap_scene.h"
#include "scenes/test_scene.h"
#include "scenes/geometry_shader_scene.h"
#include "scenes/instancing_scene.h"
#include "scenes/anti_aliasing_scene.h"

Scene* loadScene() {
	//return new MultipleLightScene;
	//return new ModelScene;
	//return new DepthScene;
	//return new StencilScene;
	//return new BlendScene;
	//return new FrameBufferScene;
	//return new CubemapScene;
	//return new TestScene;
	//return new GeometryShaderScene;
	//return new InstancingScene;
	return new AntiAliasingScene;
}

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// camera
Camera* camera;
Scene* scene;

void processInput(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void update(GLFWwindow* window, float deltaTime) {
	// ��������
	processInput(window, deltaTime);
	scene->update(window, deltaTime);
}

void onDestroy() {
	scene->destroy();
	delete(scene);
}

int main() {
	GLFWwindow* window = initWindow();

	scene = loadScene();
	camera = scene->getCamera();

	scene->init(window, SCR_WIDTH, SCR_HEIGHT);

	// �������ع�꣬�ҹ�겻���뿪����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	render_loop(window, update, onDestroy);
	return 0;
}

void processInput(GLFWwindow* window, float deltaTime) {
	// ����escʱ���ر�glfw���´�ѭ���������ʧ�ܣ����˳�����
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->ProcessKeyboard(RIGHT, deltaTime);
	}
}

bool firstMouse = true;
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;  // �������෴�ģ�y�����Ǵӵײ����������������
	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera->ProcessMouseScroll(yoffset);
}

