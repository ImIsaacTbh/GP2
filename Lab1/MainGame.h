#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <chrono>

enum class GameState{PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();
private:

	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();


	Shader* shader;
	Texture* texture;
	Mesh* fart;
	double f1, f2, f3;
	unsigned int shaderProgram;
	float counter = 0;
	Display _gameDisplay;
	GameState _gameState;
	Mesh* mesh1;
	Mesh* mesh2;
	Camera myCamera;

	//float frametime;
//	chrono::steady_clock::time_point prevFrameStart;
};

