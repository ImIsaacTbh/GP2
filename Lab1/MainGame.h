#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <chrono>

enum class GameState { PLAY, EXIT };

class Attribute
{

};

struct _object
{
	_object(int id, Transform transform, Mesh* mesh, const std::string objPath, const glm::vec3 basicColor = glm::vec3(0, 0, 0))
	{
		this->id = id;
		_transform = transform;
		_mesh = mesh;
		_mesh->LoadModel(objPath);
		_basicColor = basicColor;
	}

	int id;
	Transform _transform;
	Mesh* _mesh; 
	glm::vec3 _basicColor;

};

enum Primitive
{
	CUBE,
	ICOSPHERE,
	CAPSULE,
	MONKI
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

	// dynamic instantiation
	void CreatePrimitive(Primitive p, Shader s);
	void SetPrimitiveTransform(Transform t);

	void gameLoop();
	void startIMGUI();
	void drawGame();
	void drawAllObjects();
	void shadowPass(const std::vector<glm::vec3>& lightPositions);
	glm::mat4 CalculateLightSpaceMatrix();

	std::vector<_object> objects;
	Shader* shadowShader;
	double f1, f2, f3;
	unsigned int shaderProgram;
	float counter = 0;
	Display _gameDisplay;
	GameState _gameState;
	Camera myCamera;
	const float sens = 5;

	unsigned int fbo;
	GLuint shadowMaps[4];
	GLuint shadowFBOs[4];
	const unsigned int SHADOW_WIDTH = 2048;
	const unsigned int SHADOW_HEIGHT = 2048;

	float frametime;
	double deltaTime;
	float framesCompleted;
	chrono::high_resolution_clock::time_point prevFrameStart;
	chrono::steady_clock::time_point second;
};