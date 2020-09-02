#pragma once
#include <memory>
#include "scene.h"
#include "model.h"

#include <map>
#include "ft2build.h"
#include FT_FREETYPE_H

class FreetypeScene : public Scene {

public:
	Camera camera;
	float width;
	float height;

	unsigned int VAO;
	unsigned int VBO;

	Shader* shader;

	struct Character {
		unsigned int TextureID;  // ���������ID
		glm::ivec2 Size;  // ���δ�С
		glm::ivec2 Bearing; // �ӻ�׼�ߵ�������/������ƫ����
		unsigned int Advace;  // ԭ�����һ������ԭ��ľ���
	};
	std::map<char, Character> Characters;

	virtual Camera* getCamera() { return &camera; }

	virtual void init(GLFWwindow* window, float width, float height) {
		this->width = width;
		this->height = height;
		camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		shader = new Shader("shaders\\freetype.vs", "shaders\\freetype.frag");
		glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
		shader->use();
		shader->setMat4("projection", projection);

		// ��ʼ��FreeType��
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
		}

		FT_Face face;
		if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
			cout << "ERROR::FREETYPE: Failed to load font" << endl;
		}
		// ���������С��0�� 8��ʾ����Ŀ�ߣ����ֵΪ0��ʾͨ�������ĸ߶ȶ�̬������
		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // �����ֽڶ�������
		// ����ASCII�ַ�����ǰ128���ַ�
		for (unsigned char c = 0; c < 128; c ++) {
			// ����һ�����Σ�FT_LOAD_RENDER��ʾ����һ��8λ�ĻҶ�λͼ
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
				continue;
			}
			// ��������
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			// ��������ѡ��
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		// ����FreeType��Դ
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		initVAO();

		// ������ϣ�����Ⱦ����
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void initVAO() {
		glGenVertexArrays(1, &VAO);
		
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// ÿ���ı�����Ҫ6�����㣬ÿ������4��float����
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	virtual void update(GLFWwindow* window, float deltaTime) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// �����ɫ�������Ȼ���
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderText(*shader, "IWIN IS GREAT", 540.0f, 570.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
		
		renderText(*shader, "This is sample text pqxalhs", 25.0f, 25.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
	}

	// ��Ⱦ�ַ���
	void renderText(Shader& s, string text, float x, float y, float scale, glm::vec3 color) {
		s.use();
		s.setVec3("textColor", color);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// �����ַ����������ַ�
		string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++) {
			Character ch = Characters[*c];
			float xpos = x + ch.Bearing.x * scale;
			// ��֤p��q�����ַ���Ⱦ����׼������
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			// ��ÿ���ַ�����VBO
			float vertices[6][4] = {
				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos, ypos, 0.0, 1.0 },
				{ xpos + w, ypos, 1.0, 1.0 },

				{ xpos, ypos + h, 0.0, 0.0 },
				{ xpos + w, ypos, 1.0, 1.0 },
				{ xpos + w, ypos + h, 1.0, 0.0 },
			};

			// ���ı����ϻ�������
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// ����VBO
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// �����ı���
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// ����λ�õ���һ�����ε�ԭ�㣬��λ��1/64����
			x += (ch.Advace >> 6) * scale;
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	virtual void destroy() {

	}
};