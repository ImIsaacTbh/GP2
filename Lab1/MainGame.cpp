#include "MainGame.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include "IMGUI/imgui_impl_sdl2.h"
#include "IMGUI/imgui_impl_opengl3.h"

unsigned int indices[] = { 0, 1, 2 };
Transform transform;
GLuint defaultWhiteTexture = 0;

void CreateDefaultWhiteTexture()
{
	unsigned char white[4] = { 255, 255, 255, 255 };
	
	glGenTextures(1, &defaultWhiteTexture);
	glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

MainGame::MainGame()
{
}

MainGame::~MainGame()
{
	_gameState = GameState::PLAY;
	if (defaultWhiteTexture != 0)
	{
		glDeleteTextures(1, &defaultWhiteTexture);
	}
}

double gimmenumber()
{
	return static_cast<double>(rand()) / RAND_MAX;
}

void MainGame::run()
{
	srand(time(NULL));
	initSystems();
	counter = 0;
	vector<Vertex> things = {
		//Vertex(glm::vec3(0, 0.5, 0), glm::vec2(0.0, 0.0)),
		//Vertex(glm::vec3(-0.5, -0.5, 0), glm::vec2(0.0, 1.0)),
		//Vertex(glm::vec3(0.5, -0.5, 0), glm::vec2(1.0, 0.0))
	};
	gameLoop(); 
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay();
	SDL_SetRelativeMouseMode(SDL_TRUE);
	
	// Create default white texture for missing textures
	CreateDefaultWhiteTexture();

	glGenFramebuffers(1, &fbo);

	for(int i = 0; i < 4; i++)
	{
		glGenFramebuffers(1, &shadowFBOs[i]);
		glGenTextures(1, &shadowMaps[i]);
		glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	objects.push_back(*(new _object{0,
		Transform(glm::vec3(2, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0)),
		new Mesh("../Lab1/Resources/textureShader", "../Lab1/Resources/texture.png"), "../Lab1/Resources/monki.obj"
		}));

	objects.push_back(*(new _object{1,
		Transform(glm::vec3(-2, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0)),
		new Mesh("../Lab1/Resources/funnyColourThing", ""), "../Lab1/Resources/monki.obj"
		}));

	objects.push_back(*(new _object{2,
		Transform(glm::vec3(0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0)),
		new Mesh("../Lab1/Resources/funnyColourThing", ""), "../Lab1/Resources/cube.obj"
		}));

	objects.push_back(*(new _object{3,
		Transform(glm::vec3(0, 2, 0), glm::vec3(0.0, 0.0, glm::pi<float>()), glm::vec3(30, 1, 30)),
		new Mesh("../Lab1/Resources/textureShader", "../Lab1/Resources/bricks.jpg"), "../Lab1/Resources/plane.obj"
		}));

	objects.push_back(*(new _object{ 4,
		Transform(glm::vec3(0, 0, 0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-30, -30, -30)),
		new Mesh("../Lab1/Resources/colourShader", ""), "../Lab1/Resources/smolSPHERE.obj", glm::vec3(1.0, 0.5, 0.5)}));

	myCamera = Camera(glm::vec3(0, 0, -4), 5, 1.777777777777778, 0.1f, 10000);
	shadowShader = new Shader("../Lab1/Resources/shadowDepth");
}

void MainGame::gameLoop()
{
	second = chrono::high_resolution_clock::now();
	framesCompleted = 0;
	while (_gameState != GameState::EXIT)
	{
		if (glm::abs(chrono::duration_cast<chrono::milliseconds>(second - chrono::high_resolution_clock::now()).count() / 1000.0f) > 1)
		{
			std::cout << framesCompleted << std::endl;
			framesCompleted = 0;
			second = chrono::high_resolution_clock::now();
		}
		processInput();
		startIMGUI();
		drawGame();
		deltaTime = 0;
		deltaTime = glm::abs(chrono::duration_cast<chrono::milliseconds>(prevFrameStart - chrono::high_resolution_clock::now()).count() / 1000.0f);
		prevFrameStart = chrono::high_resolution_clock::now();
		framesCompleted++;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void MainGame::startIMGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();	
}

void MainGame::processInput()
{
	SDL_bool yesno;
	SDL_Event aaaa;
	while (SDL_PollEvent(&aaaa))
	{
		ImGui_ImplSDL2_ProcessEvent(&aaaa);
		switch (aaaa.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			myCamera.Pitch(aaaa.motion.yrel * -0.05f * sens * deltaTime);
			myCamera.RotateY(aaaa.motion.xrel * 0.05f * sens * deltaTime);
			break;
		}
	}

	auto keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_W])
	{
		myCamera.MoveForward(0.01 * 1000 * deltaTime);
	}
	if (keystate[SDL_SCANCODE_S])
	{
		myCamera.MoveForward(-0.01 * 1000 * deltaTime);
	}
	if (keystate[SDL_SCANCODE_A])
	{
		myCamera.MoveRight(-0.01 * 1000 * deltaTime);
	}
	if (keystate[SDL_SCANCODE_D]*1000*deltaTime)
	{
		myCamera.MoveRight(0.01*1000*deltaTime);
	}
	if(keystate[SDL_SCANCODE_ESCAPE])
	{
		switch (SDL_GetRelativeMouseMode())
		{
		case GL_TRUE:
			yesno = SDL_FALSE;
			break;
		case GL_FALSE:
			yesno = SDL_TRUE;
			break;
		}


		SDL_SetRelativeMouseMode(yesno);
	}
}

glm::mat4 MainGame::CalculateLightSpaceMatrix()
{
	glm::vec3 lightPos = *lightingTransform.GetPos();
	
	// Create a view matrix from the light's perspective
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	// Create an orthographic projection matrix for the light
	float near = 0.1f;
	float far = 1000.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near, far);
	
	// Combine view and projection
	return lightProjection * lightView;
}

void MainGame::shadowPass(const std::vector<glm::vec3>& lightPositions)
{
	shadowShader->Bind();

	for (int l = 0; l < lightPositions.size() && l < 4; l++)
	{
		glm::mat4 lightView = glm::lookAt(lightPositions[l], glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, 100.0f, -100.0f, 0.1f, 1000.0f);
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOs[l]);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		glEnable(GL_DEPTH_TEST);

		for (int i = 0; i < objects.size(); i++)
		{
			glm::mat4 mvp = lightSpaceMatrix * objects[i]._transform.GetModel();
			GLint mvpLoc = glGetUniformLocation(shadowShader->program, "lightSpaceMVP");
			if (mvpLoc != -1)
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
			objects[i]._mesh->Draw();
		}
	}
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainGame::drawAllObjects()
{
	// Define all lights here - add or remove as needed
	std::vector<glm::vec3> lightPositions = {
		*lightingTransform.GetPos(),
		glm::vec3(30.0f, -5.0f, 30.0f)
	};
	std::vector<glm::vec3> lightColors = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.8f, 0.6f)
	};

	// Calculate a light space matrix for each light
	std::vector<glm::mat4> lightSpaceMatrices;
	for (int l = 0; l < lightPositions.size(); l++)
	{
		glm::mat4 lightView = glm::lookAt(lightPositions[l], glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 1000.0f);
		lightSpaceMatrices.push_back(lightProjection * lightView);
	}

	for (int i = 0; i < objects.size(); i++)
	{

		objects[i]._mesh->shader->Bind();
		// Update shader - pass primary light space matrix for transform
		if (objects[i]._basicColor != glm::vec3(0, 0, 0))
		{
			objects[i]._mesh->shader->UpdateWithShadow(objects[i]._transform, myCamera, lightPositions[0], lightSpaceMatrices[0], objects[i]._basicColor);
		}
		else
		{
			objects[i]._mesh->shader->UpdateWithShadow(objects[i]._transform, myCamera, lightPositions[0], lightSpaceMatrices[0]);
		}

		// Upload all lights
		objects[i]._mesh->shader->SetLights(lightPositions, lightColors);

		// Upload all light space matrices
		objects[i]._mesh->shader->SetLightSpaceMatrices(lightSpaceMatrices);

		// Set diffuse texture
		if (objects[i]._mesh->diffuseTex != nullptr)
		{
			objects[i]._mesh->shader->SetTexture("diffuse", objects[i]._mesh->diffuseTex->GetID(), 0);
		}

		for (int l = 0; l < lightPositions.size() && l < 4; l++)
		{
			std::string name = "shadowMap[" + std::to_string(l) + "]";
			objects[i]._mesh->shader->SetTexture(name, shadowMaps[l], 3 + l);
		}

		objects[i]._mesh->Draw();
	}
}

void MainGame::drawGame()
{
	// Update light position
	lightingTransform.SetPos(glm::vec3(glm::sin(counter) * 10, -glm::abs(glm::sin(counter * 5)), glm::cos(counter) * 10));
	lightingTransform.SetRot(*lightingTransform.GetPos() - glm::vec3(0, 0, 0));

	// Create transform for the light object
	Transform lightThingTransform;
	lightThingTransform.SetPos(*lightingTransform.GetPos());
	lightThingTransform.SetRot(*lightingTransform.GetRot());
	lightThingTransform.SetScale(glm::vec3(0.5, 0.5, 0.5));

	// Update object transforms
	objects[0]._transform.SetRot(glm::vec3(0.0, counter*2, 0.0));
	objects[1]._transform.SetRot(glm::vec3(0.0, -counter*2, 0.0));
	objects[2]._transform = lightThingTransform;

	// PASS 1: Shadow mapping - render depth from light perspective
	std::vector<glm::vec3> lightPositions = {
	*lightingTransform.GetPos(),
	glm::vec3(0.0f, -5.0f, 0.0f),
	glm::vec3(30.0f, -5.0f, 0.0f)
	};
	shadowPass(lightPositions);

	// PASS 2: Render to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, *_gameDisplay.GetWidth(), *_gameDisplay.GetHeight());
	glCullFace(GL_BACK);
	_gameDisplay.clearDisplay(0.13f, 0.6f, 0.71f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// PASS 3: Final render with lighting and shadows to screen
	glViewport(0, 0, *_gameDisplay.GetWidth(), *_gameDisplay.GetHeight());
	_gameDisplay.clearDisplay(0.13f, 0.6f, 0.71f, 1.0f);
	drawAllObjects();

	counter += 0.01f;

	glEnd();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	_gameDisplay.swapBuffer();
}