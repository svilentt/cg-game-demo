#pragma once

#include <GL\glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Wall
{
public:
	enum class Orientation { XY, ZY };

	glm::vec3 Position;
	Orientation Orient;
	float Width;
	char *Name;

	Wall(glm::vec3 position, Wall::Orientation orientation, float width, char *name)
	{
		Position = position;
		Orient = orientation;
		Width = width;
		Name = name;
	}

	void draw(Shader gPlaneShader, glm::mat4 model, GLuint gVBO) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		float x, z;
		if (Orient == Orientation::XY)
		{
			x = Width;
			z = 0;
		}
		else
		{
			x = 0;
			z = Width;
		}
		model = glm::scale(model, glm::vec3(x, 2.0f, z));
		gPlaneShader.setMat4("model", model);

		if (VAO == NULL) {
			VAO = CreateWall(gVBO);
		}
		DrawWall(VAO);
		model = glm::mat4(1.0f);
	}

	bool collidesWith(glm::vec3 position)
	{
		bool collisionX, collisionZ;
		if (Wall::Orientation::XY == Orient)
		{
			collisionX = position.x <= Position.x + Width / 2 + 0.1f &&
				position.x >= Position.x - Width / 2 - 0.1f;
			collisionZ = position.z <= Position.z + 0.1f &&
				position.z >= Position.z - 0.1f;
		}
		else
		{
			collisionX = position.x <= Position.x + 0.1f &&
				position.x >= Position.x - 0.1f;
			collisionZ = position.z <= Position.z + Width / 2 + 0.1f &&
				position.z >= Position.z - Width / 2 - 0.1f;
		}
		// Collision only if on both axes
		if (collisionX && collisionZ)
		{
			printf("Clicked wall at: x = %f y = %f z = %f \n", position.x, position.y, position.z);
			return true;
		}

	}

private:
	GLuint VAO = NULL;

	GLuint CreateWall(GLuint& VBO)
	{
		//Wall
		float vertices[] = {
			//Position		   	  //Normal			    //Texture
			-0.5f, -0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f
		};

		GLuint VAO;
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

	void DrawWall(GLuint vaoID)
	{
		glBindVertexArray(vaoID);
		//glDrawElements uses the indices in the EBO to get to the vertices in the VBO
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

};