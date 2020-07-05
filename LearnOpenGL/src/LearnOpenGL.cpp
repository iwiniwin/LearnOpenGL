#include <iostream>
using namespace std;
#include "camera.h"

#include "window.h"
#include "program.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Program program;

void processInput(GLFWwindow* window, float deltaTime);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void update(GLFWwindow* window, float deltaTime) {
	// ��������
	processInput(window, deltaTime);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	// �����ɫ�������Ȼ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//// ��������Ԫ
	//glActiveTexture(GL_TEXTURE0);
	//// �����������ǰ���������Ԫ
	//glBindTexture(GL_TEXTURE_2D, program.tex0);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, program.tex1);

	// �������������󣬼����ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������
	glUseProgram(program.ID);

	glUniform3f(glGetUniformLocation(program.ID, "objectColor"), 1.0f, 0.5f, 0.31f);
	glUniform3f(glGetUniformLocation(program.ID, "lightColor"), 1.0f, 1.0f, 1.0f);

	glm::mat4 model = glm::mat4(1.0f);
	/*
		����1��uniform��λ��ֵ
		����2����Ҫ���͵ľ���ĸ���
		����3���Ƿ�Ծ�������û��������к��У�
		����4�������ľ�������
	*/
	glUniformMatrix4fv(glGetUniformLocation(program.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 view;
	// ����1�����λ�ã�����2��Ŀ��λ�ã�����3�����ϵ�����
	// cameraPos + cameraFront ��֤���������λ���Ƕ��٣������һֱ��ָ��ǰ����
	view = camera.GetViewMatrix();
	unsigned int viewLoc = glGetUniformLocation(program.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// ͶӰ����
	glm::mat4 projection;
	/*
		����һ��͸��ͶӰ����
		����1��ָ����fov��ֵ����ʾ������Ұ
		����2�������˿�߱ȣ����ӿڵĿ���Ը�����
		����3�������˽�ƽ�����������ľ���
		����4��������Զƽ�����������ľ��봩͸
	*/
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	unsigned int projectionLoc = glGetUniformLocation(program.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


	
	glBindVertexArray(program.VAO);
	/*
		���ƺ���
		����1��ָ��������Ƶ�OpenGLͼԪ������
		����2��ָ���˶����������ʼ����
		����3��ָ��������ƶ��ٸ�����
	*/
	glDrawArrays(GL_TRIANGLES, 0, 36);
	

	/*
		ָ��������������Ⱦ���ú����ӵ�ǰ�󶨵���GL_ELEMENT_ARRAY_BUFFERĿ���EBO�л�ȡ����
		����1��ָ�����Ƶ�OpenGLͼԪ������
		����2��ָ��������ƶ���ĸ���
		����3��������������
		����4��ָ��EBO�е�ƫ����
	*/
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glUseProgram(program.LightID);
	// �Ƶ�λ��
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	glUniformMatrix4fv(glGetUniformLocation(program.LightID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniformMatrix4fv(glGetUniformLocation(program.LightID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program.LightID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(program.LightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void onDestroy() {
	program.destory();
}

int main() {
	GLFWwindow* window = initWindow();

	// �������ع�꣬�ҹ�겻���뿪����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	program = initProgram();

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
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

