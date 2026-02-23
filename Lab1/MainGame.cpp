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
	shader = new Shader("../Lab1/Resources/textureShader");
	texture = new Texture("../Lab1/Resources/texture.png");

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
	mesh1 = new Mesh();
	myCamera = Camera(glm::vec3(0, 0, -4), 5, 1.777777777777778, 0.1f, 10000);
	mesh1->LoadModel("../Lab1/Resources/monki.obj");

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

	//SDL_Event aaa;
	//auto keystate = SDL_GetKeyboardState(NULL);
	//if (keystate[SDL_SCANCODE_W])
	//{
	//	myCamera->MoveForward((frametime*0.001f)*5)
	//}
}

void MainGame::drawGame()
{
	
	_gameDisplay.clearDisplay(0.13f, 0.6f, 0.71f, 1.0f);
	glEnableClientState(GL_COLOR_ARRAY);
	Transform transform;

	transform.SetPos(glm::vec3(0.0, 0.0, 0.0));
	transform.SetRot(glm::vec3(0.0, counter/10, 0.0));
	transform.SetScale(glm::vec3(1.0, 1.0, 1.0));

	shader->Bind();
	shader->Update(transform, myCamera);
	texture->Bind(0);
	mesh1->Draw();
	//mesh2->Draw();
	//if (counter > 6) counter = 0;
	counter += 0.001f;
	glEnd();
	//prevFrameStart = chrono::steady_clock::now();
	_gameDisplay.swapBuffer();
}