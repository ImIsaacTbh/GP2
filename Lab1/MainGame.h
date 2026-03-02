#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <chrono>

enum class GameState { PLAY, EXIT };

struct _object
{
	_object(int id, Transform transform, Mesh* mesh, const std::string objPath)
	{
		this->id = id;
		_transform = transform;
		_mesh = mesh;
		_mesh->LoadModel(objPath);
	}

	int id;
	Transform _transform;
	Mesh* _mesh; 
};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();
	Transform lightingTransform;
private:

	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();
	void drawAllObjects();

	std::vector<_object> objects;
	Shader* godrayShader;
	//Shader* shader;
	//Texture* texture;
	//Mesh* fart;
	double f1, f2, f3;
	unsigned int shaderProgram;
	float counter = 0;
	Display _gameDisplay;
	GameState _gameState;
	//Mesh* mesh1;
	//Mesh* mesh2;
	//Mesh* mesh3;
	//Mesh* mesh4;
	Camera myCamera;

	unsigned int fbo;

	float frametime;
	chrono::steady_clock::time_point prevFrameStart;
};