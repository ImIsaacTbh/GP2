#include "MainGame.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

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
	f1 = gimmenumber();
	f2 = gimmenumber();
	f3 = gimmenumber();
	f1 *= 1.3;
	f2 *= 1.3;
	f3 *= 1.3;
	counter = 0;
	shader = new Shader("C:\\Users\\aster\\source\\repos\\GP2-Lab1\\Lab1\\Resources\\funnyColourThing");
	texture = new Texture("C:\\Users\\aster\\source\\repos\\GP2-Lab1\\Lab1\\Resources\\pikmin-red-lay-leaf.png");

	vector<Vertex> things = {
		//Vertex(glm::vec3(-0.5, -0.5, 0), glm::vec2(0.0, 0.0)),
		//Vertex(glm::vec3(0, 0.5, 0), glm::vec2(0.0, 1.0)),
		//Vertex(glm::vec3(0.5, -0.5, 0), glm::vec2(1.0, 0.0))
	};

	double interval = 0.01;
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
	fart = new Mesh(&things[0], things.size());
	//compShaders();
	gameLoop();
}

void MainGame::compShaders()
{
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"   ourColor = aColor;\n"
		"}\0";

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(ourColor, 1.0f);\n"
		"}\n\0";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay();
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
}

void MainGame::drawGame()
{
#pragma region
	//glClearDepth(1.0);
	//_gameDisplay.clearDisplay();

	//glEnableClientState(GL_COLOR_ARRAY);

	//double interval = 0.01;
	//for (double i = 1; i < 361; i = i + interval)
	//{
	//	float x, y;
	//	x = (0.5 * sin(i));
	//	y = (0.5 * cos(i));
	//	float x2, y2;
	//	x2 = (0.5 * sin(i + interval));
	//	y2 = (0.5 * cos(i + interval));
	//	float vertices[] = {
	//	0.0f, 0.0f, 0.0f, f1, f2, f3,
	//	x, y, 0.0f, f3, f1, f2,
	//	x2, y2, 0.0f, f2, f3, f1,
	//	};

	//	unsigned int VBO, VAO;
	//	glGenVertexArrays(1, &VAO);
	//	glGenBuffers(1, &VBO);
	//	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	//	glBindVertexArray(VAO);

	//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//	// position attribute
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//	glEnableVertexAttribArray(0);
	//	// color attribute
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//	glEnableVertexAttribArray(1);

	//	glBindVertexArray(VAO);
	//	glDrawArrays(GL_TRIANGLES, 0, 3);
	//	glUseProgram(shaderProgram);
	//	glDeleteVertexArrays(1, &VAO);
	//	glDeleteBuffers(1, &VBO);
	//	glDeleteProgram(shaderProgram);
	//}
	//
	//float vertices[] = {
	//	-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	//	-0.75f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	//	-0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	//};

	//unsigned int VBO, VAO;
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	//glBindVertexArray(VAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//// color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glUseProgram(shaderProgram);
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteProgram(shaderProgram);

	//_gameDisplay.swapBuffer();
#pragma endregion
	_gameDisplay.clearDisplay();
	glEnableClientState(GL_COLOR_ARRAY);
	//Vertex things[3] = {
	//	Vertex(glm::vec3(-0.5, -0.5, 0)),
	//	Vertex(glm::vec3(0, 0.5, 0)),
	//	Vertex(glm::vec3(0.5, -0.5, 0))
	//};

	Transform transform;

	transform.SetPos(glm::vec3(0.0, sinf(counter) > 0 ? sinf(counter) : 0, 0.0));
	transform.SetRot(glm::vec3(counter < 3 && counter > 0.5 ? counter*5 : 0.80, 0.0, 180+sinf(counter)*2));
	transform.SetScale(glm::vec3(1.0,1.0,1.0));
	//transform.SetPos(glm::vec3(0.0,0.0,0.0));
	//transform.SetRot(glm::vec3(0.0, 0.0, 90.0));
	//transform.SetScale(glm::vec3(1.0, 1.0, 1.0));


	shader->Bind();
	shader->Update(transform);
	texture->Bind(0);
	fart->Draw();
	if (counter > 6) counter = 0;
	counter = counter + 0.01f;
	glEnd();
	_gameDisplay.swapBuffer();
}