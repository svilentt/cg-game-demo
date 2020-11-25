#pragma once

#include <GL\glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Cuboid.h"
#include "Chest.h"

class Torch
{
public:

	glm::vec3 Position;

	Torch(glm::vec3 position)
	{
		Position = position;
	}

	Torch(glm::vec3 position, int fireStartingFrame)
	{
		Position = position;
		torchlightCounter = fireStartingFrame;
	}

	void draw(Shader torchLightShader, Shader torchHandleShader, glm::mat4 model, GLuint* torchLightTexIDs, GLuint handleTexID) {
		model = glm::mat4(1.0f);
		
		animateFire();

		// draw fire
		glUseProgram(torchLightShader.ID);
		if (transparentVAO == NULL) {
			transparentVAO = createTransperentVAO();
		}
		glBindVertexArray(transparentVAO);
		glBindTexture(GL_TEXTURE_2D, torchLightTexIDs[torchlightCounter]);
		model = glm::translate(model, Position + glm::vec3(0.0f, -0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		torchLightShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// draw handle
		glUseProgram(torchHandleShader.ID);
		glBindTexture(GL_TEXTURE_2D, handleTexID);
		Chest torchHandle = Chest(Position + glm::vec3(0.28f, -1.0f, 0.0f), 0.05f, 0.3f, 0.05f, "TorchHandle");
		torchHandle.draw(torchHandleShader);

		model = glm::mat4(1.0f);
	}

private:
	int torchlightCounter = 0;
	int torchlightCounterSupport = 0;
	bool torchlightCounterIncrease = true;
	GLuint transparentVAO = NULL;

	void animateFire()
	{
		torchlightCounterSupport++;
		if (torchlightCounter >= 15) torchlightCounter = 0;
		if (torchlightCounter <= 0) torchlightCounterIncrease = true;
		if (torchlightCounterSupport >= 8)
		{
			if (torchlightCounterIncrease)
			{
				torchlightCounter++;
			}
			else
			{
				torchlightCounter -= 2;
			}
			torchlightCounterSupport = 0;
		}
	}

	GLuint createTransperentVAO() {
		GLuint VAO, VBO;
		float transparentVertices[] = {
			// position            // texture
			0.0f,  0.5f,  0.0f,    0.0f,  1.0f,
			0.0f, -0.5f,  0.0f,    0.0f,  0.0f,
			1.0f, -0.5f,  0.0f,    1.0f,  0.0f,

			0.0f,  0.5f,  0.0f,    0.0f,  1.0f,
			1.0f, -0.5f,  0.0f,    1.0f,  0.0f,
			1.0f,  0.5f,  0.0f,    1.0f,  1.0f
		};
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		return VAO;
	}

};