#pragma once
#include "scene.h"
#include "model.h"

extern float vertices[];
extern unsigned int indices[];
extern int verticesSize;
extern int indicesSize;

class MultipleLightScene : Scene {

public:
	void init(GLFWwindow* window, float width, float height) {
		// ������Ȳ���
		glEnable(GL_DEPTH_TEST);
		// �ر���Ȳ���
		//glDisable(GL_DEPTH_TEST);

		Shader shader("shaders\\shader.vs", "shaders\\object_shader.fs");
		Shader lightShader("shaders\\shader.vs", "shaders\\light_shader.fs");

		unsigned int VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		// ��VAO
		glBindVertexArray(VAO);

		// ��VBO�����㻺������������GL_ARRAY_BUFFER�����´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����
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
		glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);



		// ��EBO
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		// �Զ�������λ��ֵ��Ϊ���������ö�������
		glEnableVertexAttribArray(0);

		// ����
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// ��������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		unsigned int tex0 = loadTextureFromFile("textures\\container2.png");
		unsigned int tex1 = loadTextureFromFile("textures\\container2_specular.png");

		shader.use();
		//// glUniform1i���������������һ��λ��ֵ
		glUniform1i(glGetUniformLocation(shader.ID, "material.diffuse"), 0);
		shader.setInt("material.specular", 1);

		unsigned int lightVAO;
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);
		// ֻ��Ҫ��VBO��������Ҫ�ٴ�����VBO����Ϊ�������ӵ�VBO�Ѿ���������ȷ������
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// ���õƵĶ������ԣ����ڵ���˵ֻ��λ�����ݣ�
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		// �����߿�ģʽ������1��ʾӦ�õ����������ε�����ͱ��棬����2��ʾ����������
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// ���û�Ĭ��ģʽ
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// ���VBO��������ΪglVertexAttribPointer�Ѿ���VBOע��Ϊ�������԰󶨵�VBO
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		// ע�⣬��VAO����ʱ�����ܽ��EBO����ΪVAO������洢glBindBuffer�ĺ������ã�Ҳ��洢glBindBuffer�ĺ������ã�����VAO�оͲ�����EBO������
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// ���VAO
		glBindVertexArray(0);

		Program program;

		program.VAO = VAO;
		program.LightVAO = lightVAO;
		program.VBO = VBO;
		program.EBO = EBO;
		program.ID = shader.ID;
		program.LightID = lightShader.ID;
		program.tex0 = tex0;
		program.tex1 = tex1;

		return program;
	}
	void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ��������Ԫ
		glActiveTexture(GL_TEXTURE0);
		// �����������ǰ���������Ԫ
		glBindTexture(GL_TEXTURE_2D, program.tex0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, program.tex1);

		// �Ƶ�λ��
		glm::vec3 lightPos(1.0f, 0.4f, 1.0f);
		// ��Դ����
		glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);

		glm::vec3 lightColor(1.0f);
		//lightColor.x = sin(glfwGetTime() * 2.0f);
		//lightColor.y = sin(glfwGetTime() * 0.7f);
		//lightColor.z = sin(glfwGetTime() * 1.3f);

		// �������������󣬼����ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������
		glUseProgram(program.ID);
		glUniform3fv(glGetUniformLocation(program.ID, "viewPos"), 1, &camera.Position[0]);

		glUniform3f(glGetUniformLocation(program.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(program.ID, "material.shininess"), 64.0f);

		// ƽ�й�
		glUniform3fv(glGetUniformLocation(program.ID, "dirLight.direction"), 1, &lightDirection[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "dirLight.ambient"), 1, &(lightColor * glm::vec3(0.5f))[0]);
		glUniform3fv(glGetUniformLocation(program.ID, "dirLight.diffuse"), 1, &(lightColor * glm::vec3(0.4f))[0]);
		glUniform3f(glGetUniformLocation(program.ID, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		// ���Դ
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

		// �۹�
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
			����1��uniform��λ��ֵ
			����2����Ҫ���͵ľ���ĸ���
			����3���Ƿ�Ծ�������û��������к��У�
			����4�������ľ�������
		*/
		//glUniformMatrix4fv(glGetUniformLocation(program.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

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
			ָ��������������Ⱦ���ú����ӵ�ǰ�󶨵���GL_ELEMENT_ARRAY_BUFFERĿ���EBO�л�ȡ����
			����1��ָ�����Ƶ�OpenGLͼԪ������
			����2��ָ��������ƶ���ĸ���
			����3��������������
			����4��ָ��EBO�е�ƫ����
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
	void destroy() {

	}
};