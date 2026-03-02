#include "MainGame.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

unsigned int indices[] = { 0, 1, 2 };
Transform transform;

MainGame::MainGame()
{
}

MainGame::~MainGame()
{
	_gameState = GameState::PLAY;
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

	glGenFramebuffers(1, &fbo);

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
		Transform(glm::vec3(0, 2, 0), glm::vec3(0.0, 0.0, glm::pi<float>()), glm::vec3(15, 0, 15)),
		new Mesh("../Lab1/Resources/textureShader", "../Lab1/Resources/bricks.jpg"), "../Lab1/Resources/plane.obj"
		}));

	myCamera = Camera(glm::vec3(0, 0, -4), 5, 1.777777777777778, 0.1f, 10000);
	godrayShader = new Shader("../Lab1/Resources/godRays");
}

void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		processInput();
		drawGame();
	}
}

void MainGame::processInput()
{
	SDL_Event aaaa;
	while (SDL_PollEvent(&aaaa))
	{
		switch (aaaa.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
		}
	}

	SDL_Event aaa;
	auto keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_W])
	{
		myCamera.MoveForward(0.01);
	}
	if (keystate[SDL_SCANCODE_S])
	{
		myCamera.MoveForward(-0.01);
	}
	if (keystate[SDL_SCANCODE_A])
	{
		myCamera.MoveRight(-0.01);
	}
	if (keystate[SDL_SCANCODE_D])
	{
		myCamera.MoveRight(0.01);
	}
	if(keystate[SDL_SCANCODE_ESCAPE])
	{
		_gameState = GameState::EXIT;
	}

	SDL_Event mouseEvent;
	while (SDL_PollEvent(&mouseEvent))
	{
		switch (mouseEvent.type)
		{
		case SDL_MOUSEMOTION:
			myCamera.Pitch(mouseEvent.motion.yrel * -0.005f);
			myCamera.RotateY(mouseEvent.motion.xrel * 0.005f);
			break;
		}
	}
}

void MainGame::drawAllObjects()
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]._mesh->shader->Bind();
		objects[i]._mesh->shader->Update(objects[i]._transform, myCamera, *lightingTransform.GetPos());
		if (objects[i]._mesh->texture != nullptr)
		{
			objects[i]._mesh->texture->Bind(0);
		}
		objects[i]._mesh->Draw();
	}
}

void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.13f, 0.6f, 0.71f, 1.0f);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	lightingTransform.SetPos(glm::vec3(glm::sin(counter) * 10, glm::sin(counter*5), glm::cos(counter) * 10));
	lightingTransform.SetRot(*lightingTransform.GetPos() - glm::vec3(0, 0, 0));

	Transform lightThingTransform;
	lightThingTransform.SetPos(*lightingTransform.GetPos());
	lightThingTransform.SetRot(*lightingTransform.GetRot());
	lightThingTransform.SetScale(glm::vec3(0.5, 0.5, 0.5));

	objects[0]._transform.SetRot(glm::vec3(0.0, counter / 10, 0.0));
	objects[1]._transform.SetRot(glm::vec3(0.0, -counter / 10, 0.0));
	objects[2]._transform = lightThingTransform;


	godrayShader->Bind();
	for (int i = 0; i < objects.size(); i++)
	{
		godrayShader->Update(objects[i]._transform, myCamera, *lightingTransform.GetPos());
		objects[i]._mesh->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	drawAllObjects();


	counter += 0.001f;
	glEnd();
	prevFrameStart = chrono::steady_clock::now();
	_gameDisplay.swapBuffer();
	glDeleteFramebuffers(0, &fbo);
}