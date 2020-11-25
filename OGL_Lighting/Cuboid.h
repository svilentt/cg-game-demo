#pragma once

#include <GL\glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Cuboid
{
public:
	inline static void draw()
	{
		if (VAO == NULL)
		{
			VAO = createVAO();
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
private:
	static GLuint VAO;

	inline static GLuint createVAO()
	{
		GLuint VAO, VBO;
		float vertices[] = {
			//Position			     //Normal			     //Texture
			-0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,     0.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     0.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     0.0f, 0.0f
		};

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //the data comes from the currently bound GL_ARRAY_BUFFER
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //texture
		glEnableVertexAttribArray(2);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);

		return VAO;
	}
};