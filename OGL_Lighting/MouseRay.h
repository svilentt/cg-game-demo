#pragma once

#include <GL\glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Camera.h"
#include "Configuration.h"

class MouseRay
{
public:
	MouseRay(Camera cam, glm::mat4 projection)
	{
		camera = cam;
		projectionMatrix = projection;
		viewMatrix = camera.GetViewMatrix();
	}

	glm::vec3 getCurrentRay() {
		return currentRay;
	}

	void update() {
		viewMatrix = camera.GetViewMatrix();
		currentRay = calculateMouseRay();
	}

private:
	glm::vec3 currentRay = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	Camera camera;

	glm::vec3 calculateMouseRay() {
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		int win_w = Configuration::WINDOW_WIDTH;
		int win_h = Configuration::WINDOW_HEIGHT;
		float x = (2.0f * mouseX) / win_w - 1.0f;
		float y = 1.0f - (2.0f * mouseY) / win_h;
		glm::vec2 ray_nds = glm::vec2(x, y);
		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
		glm::mat4 invProjMat = glm::inverse(projectionMatrix);
		glm::vec4 eyeCoords = invProjMat * ray_clip;
		eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
		glm::mat4 invViewMat = glm::inverse(camera.GetViewMatrix());
		glm::vec4 rayWorld = invViewMat * eyeCoords;
		glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

		return rayDirection;
	}
};