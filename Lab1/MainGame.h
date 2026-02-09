#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

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


	Shader* shader;
	Texture* texture;
	Mesh* fart;
	double f1, f2, f3;
	unsigned int shaderProgram;
	float counter = 0;
	Display _gameDisplay;
	GameState _gameState;

};

