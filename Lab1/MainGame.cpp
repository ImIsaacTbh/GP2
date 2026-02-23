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
	/*f1 = gimmenumber();
	f2 = gimmenumber();
	f3 = gimmenumber();
	f1 *= 1.3;
	f2 *= 1.3;
	f3 *= 1.3;*/
	counter = 0;
	vector<Vertex> things = {
		//Vertex(glm::vec3(0, 0.5, 0), glm::vec2(0.0, 0.0)),
		//Vertex(glm::vec3(-0.5, -0.5, 0), glm::vec2(0.0, 1.0)),
		//Vertex(glm::vec3(0.5, -0.5, 0), glm::vec2(1.0, 0.0))
	};

	/*double interval = 0.01;
	for (double i = 1; i < 361; i = i + interval)
	{
		float x, y;
		x = (0.5 * sin(i));
		y = (0.5 * cos(i));
		float x2, y2;
		x2 = (0.5 * sin(i + interval));
		y2 = (0.5 * cos(i + interval));

		float coordX = ((0 + x + x2) / 3) * 1.5;
		float coordY = ((0 + y + y2) / 3) * 1.5;
		things.push_back(Vertex(glm::vec3(0, 0, 0), glm::vec2(0.0, 0.0)));
		things.push_back(Vertex(glm::vec3(x, y, 0), glm::vec2(0.5f + x, 0.5f + y)));
		things.push_back(Vertex(glm::vec3(x2, y2, 0), glm::vec2(0.5f + x2, 0.5f + y2)));
	}
	fart = new Mesh(things[0], things.size());*/
	//compShaders();
	gameLoop();
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay();
	SDL_SetRelativeMouseMode(SDL_TRUE);
	// regular textured shader / mesh
	mesh1 = new Mesh("../Lab1/Resources/textureShader", "../Lab1/Resources/texture.png");
	myCamera = Camera(glm::vec3(0, 0, -4), 5, 1.777777777777778, 0.1f, 10000);
	mesh1->LoadModel("../Lab1/Resources/monki.obj");

	mesh2 = new Mesh("../Lab1/Resources/funnyColourThing", "");
	mesh2->LoadModel("../Lab1/Resources/monki.obj");

	mesh3 = new Mesh("../Lab1/Resources/funnyColourThing", "");
	mesh3->LoadModel("../Lab1/Resources/cube.obj");

	mesh4 = new Mesh("../Lab1/Resources/textureShader", "../Lab1/Resources/bricks.jpg");
	mesh4->LoadModel("../Lab1/Resources/plane.obj");
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

void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.13f, 0.6f, 0.71f, 1.0f);
	glEnableClientState(GL_COLOR_ARRAY);

	lightingTransform.SetPos(glm::vec3(glm::sin(counter) * 10, glm::sin(counter*5), glm::cos(counter) * 10));
	lightingTransform.SetRot(*lightingTransform.GetPos() - glm::vec3(0, 0, 0));

	Transform lightThingTransform;
	lightThingTransform.SetPos(*lightingTransform.GetPos());
	lightThingTransform.SetRot(*lightingTransform.GetRot());
	lightThingTransform.SetScale(glm::vec3(0.5, 0.5, 0.5));

	Transform transform;
	transform.SetPos(glm::vec3(2, 0.0, 0.0));
	transform.SetRot(glm::vec3(0.0, counter/10, 0.0));
	transform.SetScale(glm::vec3(1.0, 1.0, 1.0));

	Transform transform2;
	transform2.SetPos(glm::vec3(-2, 0.0, 0.0));
	transform2.SetRot(glm::vec3(0.0, -counter / 10, 0.0));
	transform.SetScale(glm::vec3(1.0, 1.0, 1.0));

	Transform groundTransform;
	groundTransform.SetPos(glm::vec3(0, 2, 0));
	groundTransform.SetRot(glm::vec3(0, 0, glm::pi<float>()));
	groundTransform.SetScale(glm::vec3(15, 0 , 15));

	mesh1->shader->Bind();
	mesh1->shader->Update(transform, myCamera, *lightingTransform.GetPos());
	mesh1->texture->Bind(0);
	mesh1->Draw();

	mesh2->shader->Bind();
	mesh2->shader->Update(transform2, myCamera, *lightingTransform.GetPos());
	mesh2->Draw();

	mesh3->shader->Bind();
	mesh3->shader->Update(lightThingTransform, myCamera, *lightingTransform.GetPos());
	mesh3->Draw();

	mesh4->shader->Bind();
	mesh4->shader->Update(groundTransform, myCamera, *lightingTransform.GetPos());
	mesh4->texture->Bind(0);
	mesh4->Draw();
	//if (counter > 6) counter = 0;
	counter += 0.001f;
	glEnd();
	//prevFrameStart = chrono::steady_clock::now();
	_gameDisplay.swapBuffer();
}