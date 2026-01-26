#include "MainGame.h"
#include <iostream>
#include <string>
#include <chrono>

MainGame::MainGame()
{
}

MainGame::~MainGame()
{
	_gameState = GameState::PLAY;
}

void MainGame::run()
{
	srand(time(0));
	initSystems();
	gameLoop();
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

double gimmenumber()
{
	return static_cast<double>(rand()) / RAND_MAX;
}

void MainGame::drawGame()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear colour and depth buffer - set colour to colour defined in glClearColor

	glEnableClientState(GL_COLOR_ARRAY);

	double interval = 0.01;
	for (double i = 1; i < 361; i = i + interval)
	{
		glBegin(GL_TRIANGLES);
		glColor3f(gimmenumber(), gimmenumber(), gimmenumber());
		double x, y;
		x = (0.5 * sin(i));
		y = (0.5 * cos(i));
		double x2, y2;
		x2 = (0.5 * sin(i + interval));
		y2 = (0.5 * cos(i + interval));
		glVertex2f(0, 0);
		glVertex2f(x, y);
		glVertex2f(x2, y2);
		glEnd();
	}

	for (double i = 1; i < 360; i = i + interval)
	{
		glBegin(GL_TRIANGLES);
		glColor3f(0.4f, 0.4f, 0.4f);
		double x, y;
		x = (0.1 * sin(i));
		y = (0.1 * cos(i));
		double x2, y2;
		x2 = (0.1 * sin(i + interval));
		y2 = (0.1 * cos(i + interval));
		glVertex2f(0, 0);
		glVertex2f(x, y);
		glVertex2f(x2, y2);
		glEnd();
	}

	glColor3f(1, 1, 0);
	glBegin(GL_QUADS);
	glVertex2f(0.5, 0.5);
	glVertex2f(0.5, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 0.5);
	glEnd();


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
	unsigned int shaderProgram = glCreateProgram();
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
	
	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.75f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(shaderProgram);

	//glVertex2f(-0.5, -0.5);
	//glVertex2f(0.5, -0.5);
	//glVertex2f(0, 0.5);

	_gameDisplay.swapBuffer();
}