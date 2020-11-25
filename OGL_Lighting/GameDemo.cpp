#include <GL\glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION //if not defined the function implementations are not included
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Wall.h"
#include "Chest.h"
#include "MouseRay.h"
#include "Cuboid.h"
#include "Torch.h"
#include "Configuration.h"

bool init();
bool initGL();
void render();
GLuint CreatePlane(float, GLuint&);
void DrawPlane(GLuint);
bool LoadTexture(const char*, GLuint&);
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//shaders
Shader gMultiLightsShader, gTorchlightShader;

//static vars
GLuint Cuboid::VAO = NULL;

//Textures
GLuint wallTexID, floorTexID, chestTexID;
GLuint torchLightTexIDs[16];

//OpenGL ids
GLuint gVAOp, gVBOp;

// camera
Camera camera(glm::vec3(0.0f, 0.6f, 5.5f));
float lastX = -1;
float lastY = -1;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPositions[] = {
	glm::vec3(-0.95f, 1.5f, 1.0f),
	glm::vec3(2.0f, 1.5f, -1.95f),
	glm::vec3(1.05f, 1.5f, 1.0f),
	glm::vec3(3.0f, 1.5f, 0.05f),
	glm::vec3(7.0f, 1.5f,  -1.95f),
	glm::vec3(5.75f, 1.5f,  -7.95f)
};

// Walls
std::vector<Wall> walls = { 
	Wall(glm::vec3(0.0f, 0.5f,  6.0f), Wall::Orientation::XY, 2.0f, "Wall 0"),
	Wall(glm::vec3(-1.0f, 0.5f,  4.0f), Wall::Orientation::ZY, 4.0f, "Wall 1"),
	Wall(glm::vec3(-1.0f, 0.5f,  0.0f), Wall::Orientation::ZY, 4.0f, "Wall 1"),
	Wall(glm::vec3(0.5f, 0.5f, -2.0f), Wall::Orientation::XY, 3.0f, "Wall 2"),
	Wall(glm::vec3(3.5f, 0.5f, -2.0f), Wall::Orientation::XY, 3.0f, "Wall 2"),
	Wall(glm::vec3(1.0f, 0.5f,  1.0f), Wall::Orientation::ZY, 2.0f, "Wall 3"),
	Wall(glm::vec3(1.0f, 0.5f,  4.0f), Wall::Orientation::ZY, 4.0f, "Wall 3"),
	Wall(glm::vec3(2.5f, 0.5f,  0.0f), Wall::Orientation::XY, 3.0f, "Wall 4"),
	Wall(glm::vec3(5.5f, 0.5f,  0.0f), Wall::Orientation::XY, 3.0f, "Wall 4"),
	Wall(glm::vec3(5.0f, 0.5f,  -4.0f), Wall::Orientation::ZY, 4.0f, "Wall 5"),
	Wall(glm::vec3(5.0f, 0.5f,  -7.0f), Wall::Orientation::ZY, 2.0f, "Wall 5"),
	Wall(glm::vec3(7.0f, 0.5f,  -1.0f), Wall::Orientation::ZY, 2.0f, "Wall 6"),
	Wall(glm::vec3(7.0f, 0.5f,  -4.0f), Wall::Orientation::ZY, 4.0f, "Wall 6"),
	Wall(glm::vec3(7.0f, 0.5f,  -7.0f), Wall::Orientation::ZY, 2.0f, "Wall 6"),
	Wall(glm::vec3(6.0f, 0.5f,  -8.0f), Wall::Orientation::XY, 2.0f, "Wall 7")
};

// Jump
bool isGoingUp = false;
bool isGoingDown = false;

//event handlers
void HandleKeyDown(const SDL_KeyboardEvent& key);
void HandleMouseMotion(const SDL_MouseMotionEvent& motion);
void HandleMouseWheel(const SDL_MouseWheelEvent& wheel);
void HandleMousePress(const SDL_MouseButtonEvent& button);
void HandleMouseRelease(const SDL_MouseButtonEvent& button);
void HandleJump();

//event handler variables
bool RMBheld = false;
bool mouseClicked = false;

//game objects
std::vector<Chest> chests = {
	Chest(glm::vec3(6.0f, -0.25f,  -7.5f), 0.5f, 0.5f, 0.5f, "Chest 1")
};
std::vector<Torch> torches = {
	Torch(lightPositions[0] - glm::vec3(0.23f, 0.0f, 0.0f), 2),
	Torch(lightPositions[1], 6),
	Torch(lightPositions[2] - glm::vec3(0.40f, 0.0f, 0.0f), 11),
	Torch(lightPositions[3] - glm::vec3(0.0f, 0.0f, 0.10f), 14),
	Torch(lightPositions[4] - glm::vec3(0.40f, 0.0f, 0.0f), 3),
	Torch(lightPositions[5] + glm::vec3(0.0f, 0.0f, 0.05f), 8)
};

int main(int argc, char* args[])
{
	init();

	SDL_Event e;
	//While application is running
	bool quit = false;
	while (!quit)
	{
		// per-frame time logic
		// --------------------
		float currentFrame = SDL_GetTicks() / 1000.0f ;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else {
					HandleKeyDown(e.key);
				}
				break;
			case SDL_MOUSEMOTION:
				if(RMBheld)
					HandleMouseMotion(e.motion);
				break;
			case SDL_MOUSEWHEEL:
				HandleMouseWheel(e.wheel);
				break;
			case SDL_MOUSEBUTTONDOWN:
				HandleMousePress(e.button);
				break;
			case SDL_MOUSEBUTTONUP:
				HandleMouseRelease(e.button);
				break;
			}
		}

		HandleJump();

		//Render quad
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	close();

	return 0;
}

void HandleKeyDown(const SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym)
	{
	case SDLK_w:
		camera.ProcessKeyboard(FORWARD, deltaTime, walls);
		break;
	case SDLK_s:
		camera.ProcessKeyboard(BACKWARD, deltaTime, walls);
		break;
	case SDLK_a:
		camera.ProcessKeyboard(LEFT, deltaTime, walls);
		break;
	case SDLK_d:
		camera.ProcessKeyboard(RIGHT, deltaTime, walls);
		break;
	case SDLK_q:
		printf("Camera position at: x = %f y = %f z = %f \n", camera.Position.x, camera.Position.y, camera.Position.z);
		break;
	case SDLK_SPACE:
		if(!isGoingUp && !isGoingDown)
			isGoingUp = true;
		break;
	}
}

void HandleMouseMotion(const SDL_MouseMotionEvent& motion)
{
	if (firstMouse)
	{
		lastX = motion.x;
		lastY = motion.y;
		firstMouse = false;
	}
	else
	{
		camera.ProcessMouseMovement(motion.x - lastX, lastY - motion.y);
		lastX = motion.x;
		lastY = motion.y;
	}
	
	int win_w = Configuration::WINDOW_WIDTH;
	int win_h = Configuration::WINDOW_HEIGHT;
	float x = motion.x;
	float y = motion.y;

	if (x < 300 || x > win_w - 300) {  //you can use values other than 100 for the screen edges if you like, kind of seems to depend on your mouse sensitivity for what ends up working best
		lastX = win_w / 2;   //centers the last known position, this way there isn't an odd jump with your cam as it resets
		lastY = win_h / 2;
		SDL_WarpMouseInWindow(gWindow,win_w / 2, win_h / 2);  //centers the cursor
	}
	else if (y < 300 || y > win_h - 300) {
		lastX = win_w / 2;
		lastY = win_h / 2;
		SDL_WarpMouseInWindow(gWindow,win_w / 2, win_h / 2);
	}
}

void HandleMouseWheel(const SDL_MouseWheelEvent& wheel)
{
	camera.ProcessMouseScroll(wheel.y);
}

void HandleMousePress(const SDL_MouseButtonEvent& button)
{
	switch (button.button)
	{
	case SDL_BUTTON_RIGHT:
		RMBheld = true;
		break;

	case SDL_BUTTON_LEFT:
		int mouse_x;
		int mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);
		printf("Mouse position at: x = %d y = %d\n", mouse_x, mouse_y);
		mouseClicked = true;
		break;
	}
}

void HandleMouseRelease(const SDL_MouseButtonEvent& button)
{
	switch (button.button)
	{
	case SDL_BUTTON_RIGHT:
		RMBheld = false;
		break;
	}
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.3
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);


		//Create window
		gWindow = SDL_CreateWindow("Game Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Configuration::WINDOW_WIDTH, Configuration::WINDOW_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN*/);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	glewInit();

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		success = false;
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!LoadTexture("./textures/ulrick-wery-tileableset-brickwallmoss.jpg", wallTexID))
	{
		printf("Could not load texture \"ulrick-wery-tileableset-brickwallmoss.jpg\"");
	}	
	if (!LoadTexture("./textures/ulrick-wery-tileableset-plasterwall.jpg", floorTexID))
	{
		printf("Could not load texture \"ulrick-wery-tileableset-plasterwall.jpg\"");
	}
	if (!LoadTexture("./textures/wood01_zpsae92e1d8.jpg", chestTexID))
	{
		printf("Could not load texture \"wood01_zpsae92e1d8.jpg\"");
	}
	for (int i = 0; i < 16; i++)
	{
		std::ostringstream oss;
		oss << "./textures/torch/torchlight/torch_light_" << i << ".png";
		std::string torchLightTextureLocation = oss.str();
		if (!LoadTexture(torchLightTextureLocation.c_str(), torchLightTexIDs[i]))
		{
			printf("Could not load texture \"%s\"", torchLightTextureLocation);
		}
	}
	gMultiLightsShader.Load("./shaders/multiLights.vert", "./shaders/multiLights.frag");
	gTorchlightShader.Load("./shaders/torch.vert", "./shaders/torch.frag");
	
	gVAOp = CreatePlane(1.0f, gVBOp);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //other modes GL_FILL, GL_POINT

	return success;
}

void close()
{
	//delete GL programs, buffers and objects
	glDeleteProgram(gMultiLightsShader.ID);
	glDeleteProgram(gTorchlightShader.ID);

	glDeleteVertexArrays(1, &gVAOp);

	glDeleteBuffers(1, &gVBOp);

	//Delete OGL context
	SDL_GL_DeleteContext(gContext);
	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void render()
{
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat3 normalMat = glm::transpose(glm::inverse(model));

	  /******************************************************************/
	 /*  Configure the shader for objects affected by multiple lights  */
	/******************************************************************/
	glUseProgram(gMultiLightsShader.ID);
	//transformations
	gMultiLightsShader.setMat4("model", model);
	gMultiLightsShader.setMat4("view", view);
	gMultiLightsShader.setMat4("proj", proj);
	gMultiLightsShader.setMat3("normalMat", normalMat);
	//lighting
	gMultiLightsShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	gMultiLightsShader.setVec3("lightColor", 1.0f, 0.71f, 0.2f);
	for (int i = 0; i < 6; i++)
	{
		std::ostringstream oss;
		oss << "lightPositions[" << i << "]";
		std::string lightPosition = oss.str();
		gMultiLightsShader.setVec3(lightPosition, lightPositions[i]);
	}
	gMultiLightsShader.setVec3("viewPos", camera.Position);

	//gMultiLightsShader.use(); //we have to use the shader before setting uniforms
	gMultiLightsShader.setInt("Texture1", 0); //tell the shader that the sampler Texture1 should take its data from texture unit 0 (GL_TEXTURE0)


	  /*******************************/
	 /*  Draw bottom and top plane  */
	/*******************************/
	model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexID);

	gMultiLightsShader.setMat4("model", model);
	DrawPlane(gVAOp);

	model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
	gMultiLightsShader.setMat4("model", model);
	DrawPlane(gVAOp);

	  /************************************/
	 /*	Check if any chest was clicked  */
	/************************************/
	MouseRay mouseRay = MouseRay(camera, proj);
	mouseRay.update();
	glm::vec3 mouseRayShot = mouseRay.getCurrentRay() / 10.0f;

	if (mouseClicked)
	{
		glm::vec3 tmp = camera.Position;
		bool hasCollided = false;
		bool isTooFar = false;
		float maxDistance = 1.0f;
		do
		{
			for (int i = 0; i < chests.size(); i++)
			{
				if (chests.at(i).isClicked(tmp))
				{
					chests.at(i).open();
					hasCollided = true;
					break;
				}
			}
			tmp += mouseRayShot;
		} while (!hasCollided && !isTooFar);
		mouseClicked = false;
	}

	  /************************************/
	 /*  Draw chests, walls and torches  */
	/************************************/

	glBindTexture(GL_TEXTURE_2D, chestTexID);

	for (int i = 0; i < chests.size(); i++)
	{
		chests.at(i).draw(gMultiLightsShader);
		chests.at(i).collidesWith(camera.Position);
	}

	glBindTexture(GL_TEXTURE_2D, wallTexID);
	
	for (Wall wall : walls)
	{
		wall.draw(gMultiLightsShader, model, gVBOp);
	}

	gTorchlightShader.use();
	gTorchlightShader.setInt("Texture1", 0);
	gTorchlightShader.setMat4("view", view);
	gTorchlightShader.setMat4("proj", proj);
	for (unsigned int i = 0; i < torches.size(); i++) {
		torches.at(i).draw(gTorchlightShader, gMultiLightsShader, model, torchLightTexIDs, chestTexID);
	}
}

GLuint CreatePlane(float width, GLuint& VBO)
{
	//Floor
	float vertices[] = {
		//Position			 //Normal			  //Texture
		-0.5f,-0.5f, 0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
		 0.5f,-0.5f, 0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		-0.5f,-0.5f,-0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
		-0.5f,-0.5f,-0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
		 0.5f,-0.5f,-0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
		 0.5f,-0.5f, 0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f
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

void DrawPlane(GLuint vaoID)
{
	glBindVertexArray(vaoID);
	//glDrawElements uses the indices in the EBO to get to the vertices in the VBO
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

bool LoadTexture(const char* filename, GLuint& texID)
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //these are the default values for warping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// read the texture
	GLint width, height, channels;
	stbi_set_flip_vertically_on_load(true); //flip the image vertically while loading
	unsigned char* img_data = stbi_load(filename, &width, &height, &channels, 0); //read the image data

	if (img_data)
	{   //3 channels - rgb, 4 channels - RGBA
		GLenum format;
		switch (channels)
		{
		case 4:
			format = GL_RGBA;
			break;
		default:
			format = GL_RGB;
			break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture\n");
	}
	stbi_image_free(img_data);

	return true;
}

void HandleJump()
{
	if (isGoingUp)
	{
		camera.ProcessJump(UP, deltaTime);
		if (camera.Position.y > 1.4f)
		{
			isGoingDown = true;
			isGoingUp = false;
		}
	}
	if (isGoingDown)
	{
		camera.ProcessJump(DOWN, deltaTime);
		if (camera.Position.y <= 0.6f)
		{
			camera.Position.y = 0.6f;
			isGoingDown = false;
			isGoingUp = false;
		}
	}
}


