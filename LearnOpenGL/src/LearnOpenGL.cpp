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
	// 处理输入
	processInput(window, deltaTime);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// 清除颜色缓冲和深度缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 激活纹理单元
	glActiveTexture(GL_TEXTURE0);
	// 绑定这个纹理到当前激活的纹理单元
	glBindTexture(GL_TEXTURE_2D, program.tex0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, program.tex1);
	
	// 灯的位置
	glm::vec3 lightPos(1.0f, 0.4f, 1.0f);
	// 光源方向
	glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);

	glm::vec3 lightColor(1.0f);
	//lightColor.x = sin(glfwGetTime() * 2.0f);
	//lightColor.y = sin(glfwGetTime() * 0.7f);
	//lightColor.z = sin(glfwGetTime() * 1.3f);

	// 激活这个程序对象，激活后，每个着色器调用和渲染调用都会使用这个程序对象
	glUseProgram(program.ID);
	glUniform3fv(glGetUniformLocation(program.ID, "viewPos"), 1, &camera.Position[0]);
	
	glUniform3f(glGetUniformLocation(program.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program.ID, "material.shininess"), 64.0f);

	// 平行光
	glUniform3fv(glGetUniformLocation(program.ID, "dirLight.direction"), 1, &lightDirection[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "dirLight.ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "dirLight.diffuse"), 1, &(lightColor * glm::vec3(0.4f))[0]);
	glUniform3f(glGetUniformLocation(program.ID, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

	// 点光源
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[0].position"), 1, &pointLightPositions[0][0]);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[0].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[0].quadratic"), 0.032f);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[0].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[0].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
	glUniform3f(glGetUniformLocation(program.ID, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);

	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[1].position"), 1, &pointLightPositions[1][0]);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[1].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[1].quadratic"), 0.032f);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[1].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[1].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
	glUniform3f(glGetUniformLocation(program.ID, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);

	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[2].position"), 1, &pointLightPositions[2][0]);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[2].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[2].quadratic"), 0.032f);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[2].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[2].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
	glUniform3f(glGetUniformLocation(program.ID, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);

	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[3].position"), 1, &pointLightPositions[3][0]);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[3].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(program.ID, "pointLights[3].quadratic"), 0.032f);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[3].ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "pointLights[3].diffuse"), 1, &(lightColor * glm::vec3(0.8f))[0]);
	glUniform3f(glGetUniformLocation(program.ID, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);

	// 聚光
	glUniform3fv(glGetUniformLocation(program.ID, "spotLight.position"), 1, &camera.Position[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "spotLight.direction"), 1, &camera.Front[0]);
	glUniform1f(glGetUniformLocation(program.ID, "spotLight.cutoff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(program.ID, "spotLight.outercutoff"), glm::cos(glm::radians(15.0f)));
	glUniform3fv(glGetUniformLocation(program.ID, "spotLight.ambient"), 1, &(lightColor * glm::vec3(0.0f))[0]);
	glUniform3fv(glGetUniformLocation(program.ID, "spotLight.diffuse"), 1, &(lightColor * glm::vec3(1.0f))[0]);
	glUniform3f(glGetUniformLocation(program.ID, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(program.ID, "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(program.ID, "spotLight.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(program.ID, "spotLight.quadratic"), 0.032f);

	

	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	/*
		参数1，uniform的位置值
		参数2，将要发送的矩阵的个数
		参数3，是否对矩阵进行置换（交换行和列）
		参数4，真正的矩阵数据
	*/
	//glUniformMatrix4fv(glGetUniformLocation(program.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 view;
	// 参数1，相机位置，参数2，目标位置，参数3，向上的向量
	// cameraPos + cameraFront 保证无论摄像机位置是多少，摄像机一直是指向前方的
	view = camera.GetViewMatrix();
	unsigned int viewLoc = glGetUniformLocation(program.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// 投影矩阵
	glm::mat4 projection;
	/*
		构造一个透视投影矩阵
		参数1，指定了fov的值，表示的是视野
		参数2，设置了宽高比，由视口的宽除以高所得
		参数3，设置了近平面距离摄像机的距离
		参数4，设置了远平面距离摄像机的距离穿透
	*/
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	unsigned int projectionLoc = glGetUniformLocation(program.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


	
	glBindVertexArray(program.VAO);
	/*
		绘制函数
		参数1，指明打算绘制的OpenGL图元的类型
		参数2，指定了顶点数组的起始索引
		参数3，指定打算绘制多少个顶点
	*/
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 model;
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(program.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	

	/*
		指明从索引缓冲渲染，该函数从当前绑定到的GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取索引
		参数1，指明绘制的OpenGL图元的类型
		参数2，指定打算绘制顶点的个数
		参数3，是索引的类型
		参数4，指定EBO中的偏移量
	*/
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glUseProgram(program.LightID);
	glUniformMatrix4fv(glGetUniformLocation(program.LightID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program.LightID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(program.LightVAO);
	for (int i = 0; i < 4; i++) {
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, pointLightPositions[i]);
		lightModel = glm::scale(lightModel, glm::vec3(0.04f));
		glUniformMatrix4fv(glGetUniformLocation(program.LightID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void onDestroy() {
	program.destory();
}

int main() {
	GLFWwindow* window = initWindow();

	// 设置隐藏光标，且光标不会离开窗口
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	program = initProgram();

	render_loop(window, update, onDestroy);

	return 0;
}

void processInput(GLFWwindow* window, float deltaTime) {
	// 按下esc时，关闭glfw，下次循环条件检测失败，会退出程序
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
	float yoffset = lastY - ypos;  // 这里是相反的，y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

