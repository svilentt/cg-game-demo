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
#include "Interactive.h"

class Chest: public Interactive
{
public:
	enum class State { CLOSED, OPENING, OPEN };

	glm::vec3 Position;
	float Width;
	float Height;
	float Depth;
	char *Name;

	Chest(glm::vec3 position, float width, float height, float depth, char *name)
	{
		Position = position;
		Width = width;
		Height = height;
		Depth = depth;
		Name = name;
	}

	void draw(Shader shader) {
		glm::mat4 model = glm::mat4(1.0f);
		
		if (state == State::OPENING) {
			animateOpening();
		}

		// bottom part
		model = glm::translate(model, Position);
		model = glm::scale(model, glm::vec3(Width, Height * 2 / 3, Depth));
		shader.setMat4("model", model);
		Cuboid::draw();

		// top part
		model = glm::mat4(1.0f);
		model = glm::translate(model, Position + glm::vec3(0.0f, Height * moveUp + Height * 1 / 3, -Depth * moveBack));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(Width, Height * 1 / 3, Depth));
		shader.setMat4("model", model);
		Cuboid::draw();
	}

	bool isClicked(glm::vec3 position)
	{
		if (position.x <= Position.x + Width / 2
			&& position.x >= Position.x - Width / 2
			&& position.y <= Position.y + Height / 2
			&& position.y >= Position.y - Height / 2
			&& position.z <= Position.z + Depth / 2
			&& position.z >= Position.z - Depth / 2)
		{
			printf("Chest selected at at: x = %f y = %f z = %f \n", position.x, position.y, position.z);
			return true;
		}
		return false;
	}

	bool collidesWith(glm::vec3 position)
	{
		if (position.x <= Position.x + Width/2
			&& position.x >= Position.x - Width/2
			&& position.z <= Position.z + Depth / 2
			&& position.z >= Position.z - Depth / 2)
		{
			printf("Collided with chest at: x = %f y = %f z = %f \n", position.x, position.y, position.z);
			return true;
		}
		return false;
	}

	void open() 
	{
		if (state == State::CLOSED)
		{
			state = State::OPENING;
		}
	}

private:
	State state = State::CLOSED;
	float maxOpeningDegree = -60.0f;
	float openingSpeed = 0.75f;
	float backSpeed = 2.0f / 7.0f / (abs(maxOpeningDegree) / openingSpeed);
	float moveBack = 0.0f;
	float upSpeed = 2.0f / 5.0f / (abs(maxOpeningDegree) / openingSpeed);
	float moveUp = 0.0f;
	float angle = 0.0f;

	void animateOpening()
	{
		angle -= openingSpeed;
		if (angle <= maxOpeningDegree)
		{
			state = State::OPEN;
		}
		moveBack += backSpeed;
		moveUp += upSpeed;
	}

};