#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include "Display.h" 

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
	void compShaders();

	double f1, f2, f3;
	unsigned int shaderProgram;
	Display _gameDisplay;
	GameState _gameState;

};

