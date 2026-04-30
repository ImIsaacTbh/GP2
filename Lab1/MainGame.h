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
		snprintf(name, sizeof(name), "Object %d", id);
		_transform = transform;
		_mesh = mesh;
		_mesh->LoadModel(objPath);
		_basicColor = basicColor;
	}

	int id;
	char name[64];
	Transform _transform;
	Mesh* _mesh; 
	glm::vec3 _basicColor;
};

enum Primitive
{
	CUBE,
	ICOSPHERE,
	UVSPHERE,
	CAPSULE,
	PLANE,
	MONKI,
	LUCIO
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
	void MainGame::CreatePrimitive(Primitive type, const std::string& shaderPath, const std::string& texturePath, const glm::vec3 scale = glm::vec3(1))
	{
		std::string meshPath;
		const char* defaultName;
		switch (type)
		{
		case CUBE:			meshPath = "../Lab1/Resources/cube.obj";		defaultName = "Cube";		break;
		case ICOSPHERE:		meshPath = "../Lab1/Resources/smolSPHERE.obj";	defaultName = "ICOSphere";	break;
		case PLANE:			meshPath = "../Lab1/Resources/plane.obj";		defaultName = "Plane";		break;
		case MONKI:			meshPath = "../Lab1/Resources/monki.obj";		defaultName = "Monki";		break;
		case UVSPHERE:		meshPath = "../Lab1/Resources/uvsphere.obj";	defaultName = "UVSphere";	break;
		case LUCIO:			meshPath = "../Lab1/Resources/lucio.obj";		defaultName = "Lucio";		break;
		default:			meshPath = "../Lab1/Resources/cube.obj";		defaultName = "Cube";		break;
		}

		int newId = objects.empty() ? 0 : objects.back().id + 1;

		_object obj = *(new _object{ newId, Transform(glm::vec3(0), glm::vec3(0), scale), new Mesh(shaderPath, texturePath), meshPath });
		snprintf(obj.name, sizeof(obj.name), "%s %d", defaultName, newId);
		objects.push_back(obj);
	}

	void SetPrimitiveTransform(Transform t);

	void gameLoop();
	void startIMGUI();
	void drawGame();
	void drawAllObjects();
	void shadowPass(const std::vector<glm::vec3>& lightPositions);

	std::vector<std::string> GetAllImagesInResources();
	std::vector<std::string> GetAllShadersInResources();

	glm::mat4 CalculateLightSpaceMatrix();

	int selectedObject = -1;
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
	float framerate;
	float framesCompleted;
	chrono::high_resolution_clock::time_point prevFrameStart;
	chrono::steady_clock::time_point second;
};