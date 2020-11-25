#pragma once
//camera class from learnopengl.com

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Wall.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 4.5f;
const float SENSITIVITY = 0.3f;
const float ZOOM = 45.0f;

const float UP_SPEED = 2.5f;
const float DOWN_SPEED = 3.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime, std::vector<Wall> walls)
	{
		glm::vec3 nextPosition;
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
		{
			nextPosition = Position + Front * glm::vec3(1.0f, 0.0f, 1.0f) * velocity;
			if (!collidesWithWalls(nextPosition, walls))
				Position = nextPosition;
			else
				Position -= Front * glm::vec3(1.0f, 0.0f, 1.0f) * velocity;
		}
		if (direction == BACKWARD)
		{
			nextPosition = Position - Front * glm::vec3(1.0f, 0.0f, 1.0f) * velocity;
			if (!collidesWithWalls(nextPosition, walls))
				Position = nextPosition;
			else
				Position += Front * glm::vec3(1.0f, 0.0f, 1.0f) * velocity;
		}
		if (direction == LEFT)
		{
			nextPosition = Position - Right * velocity;
			if (!collidesWithWalls(nextPosition, walls))
				Position = nextPosition;
			else
				Position += Right * velocity;
		}
		if (direction == RIGHT)
		{
			nextPosition = Position + Right * velocity;
			if (!collidesWithWalls(nextPosition, walls))
				Position = nextPosition;
			else
				Position -= Right * velocity;
		}
	}

	bool collidesWithWalls(glm::vec3 nextPosition, std::vector<Wall> walls)
	{
		for (Wall wall : walls)
		{
			bool collisionX, collisionZ;
			if (Wall::Orientation::XY == wall.Orient)
			{
				collisionX = nextPosition.x <= wall.Position.x + wall.Width / 2 + 0.1f &&
					nextPosition.x >= wall.Position.x - wall.Width / 2 - 0.1f;
				collisionZ = nextPosition.z <= wall.Position.z + 0.1f &&
					nextPosition.z >= wall.Position.z - 0.1f;
			}
			else 
			{
				collisionX = nextPosition.x <= wall.Position.x + 0.1f &&
					nextPosition.x >= wall.Position.x - 0.1f;
				collisionZ = nextPosition.z <= wall.Position.z + wall.Width / 2 + 0.1f &&
					nextPosition.z >= wall.Position.z - wall.Width / 2 - 0.1f;
			}
			// Collision only if on both axes
			if (collisionX && collisionZ)
			{
				printf("Collision at: x = %f y = %f z = %f \n", nextPosition.x, nextPosition.y, nextPosition.z);
				return true;
			}
		}
		return false;
	}

	void ProcessJump(Camera_Movement direction, float deltaTime)
	{
		if (direction == UP)
		{
			float velocity = UP_SPEED * deltaTime;
			Position.y += velocity;
		}

		if (direction == DOWN)
		{
			float velocity = DOWN_SPEED * deltaTime;
			Position.y -= velocity;
		}
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

