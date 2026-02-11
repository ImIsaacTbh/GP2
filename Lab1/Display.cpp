#include "Display.h"
//#include <conio.h>

Display::Display()
{
	_window = nullptr; //initialise to generate null access violation for debugging. 
	_screenWidth = 800;
	_screenHeight = 800; 
}

Display::~Display()
{
}

// void Display::returnError(std::string errorString)
// {
// 	std::cout << (errorString + "\n" + "Press Any Key To Quit...");
// 	while (_getch() < 0) {}
// 	SDL_Quit();
// 	exit(0);
// }

void Display::swapBuffer()
{
	SDL_GL_SwapWindow(_window);
}

void Display::clearDisplay()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear colour and depth buffer - set colour to colour defined in glClearColor

}

void Display::initDisplay()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); //Min no of bits used to diplay colour
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // set up double buffer   
	_window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(_window);
	GLenum error = glewInit();
	if (_window == nullptr) returnError("Failed to create window.");
	if (context == nullptr) returnError("Failed to create openGL context.");
	if (error != GLEW_OK) returnError("Failed to initalise GLEW.");
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}